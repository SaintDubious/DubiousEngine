#include "Collision_strategy_open_cl.h"
#include "Physics_object.h"
#include "Contact_manifold.h"
#include "Physics_model.h"

#include "Broad_phase.cl"

#include <set>
#include <future>
#include <iostream>

#pragma warning(disable : 4503)  // decorated name length exceeded, name was truncated
namespace Dubious {
namespace Physics {

Collision_strategy_open_cl::Collision_strategy_open_cl(float        manifold_persistent_threshold,
                                                       float        manifold_movement_threshold,
                                                       unsigned int collisions_per_thread,
                                                       int          cl_broadphase_work_group_size)
    : m_manifold_persistent_threshold(manifold_persistent_threshold)
    , m_manifold_movement_threshold(manifold_movement_threshold)
    , m_collisions_per_thread(collisions_per_thread)
    , m_cl_broadphase_work_group_size(cl_broadphase_work_group_size)
{
    bool opencl_available;
    std::tie(opencl_available, m_platform_id, m_device_id) = Utility::Open_cl::setup();
    if (!opencl_available) {
        throw std::runtime_error("Attempted to use OpenCL, but it is not supported");
    }
    m_context             = Utility::Open_cl::create_context(m_platform_id, m_device_id);
    m_command_queue       = Utility::Open_cl::create_command_queue(m_context, m_device_id);
    m_broad_phase_program = Utility::Open_cl::create_program(broad_phase, m_context, m_device_id);
    m_broad_phase_inner_kernel =
        Utility::Open_cl::create_kernel(m_broad_phase_program, "broad_phase_inner");
    m_broad_phase_outer_kernel =
        Utility::Open_cl::create_kernel(m_broad_phase_program, "broad_phase_outer");

    size_t ret_size;
    size_t work_group_size;
    cl_int rc;
    rc =
        clGetKernelWorkGroupInfo(m_broad_phase_inner_kernel, m_device_id, CL_KERNEL_WORK_GROUP_SIZE,
                                 sizeof(size_t), &work_group_size, &ret_size);
    if (CL_SUCCESS != rc) {
        throw std::runtime_error("clGetKernelWorkGroupInfo");
    }
    std::cout << "CL_KERNEL_WORK_GROUP_SIZE inner: " << work_group_size << "\n";
    rc =
        clGetKernelWorkGroupInfo(m_broad_phase_outer_kernel, m_device_id, CL_KERNEL_WORK_GROUP_SIZE,
                                 sizeof(size_t), &work_group_size, &ret_size);
    if (CL_SUCCESS != rc) {
        throw std::runtime_error("clGetKernelWorkGroupInfo");
    }
    std::cout << "CL_KERNEL_WORK_GROUP_SIZE outer: " << work_group_size << "\n";

    if (cl_broadphase_work_group_size & 0x01) {
        throw std::runtime_error(
            "Really?!?!? Is it too much to ask to have an even cl_broadphase_work_group_size?");
    }
    size_t max_results    = cl_broadphase_work_group_size * (cl_broadphase_work_group_size - 1) / 2;
    m_broad_phase_objects = new cl_float[4 * cl_broadphase_work_group_size];
    m_broad_phase_results = new cl_char[max_results];

    m_broad_phase_buffer_obj_a = Utility::Open_cl::create_buffer(
        m_context, CL_MEM_READ_ONLY, cl_broadphase_work_group_size * sizeof(cl_float) * 4);
    m_broad_phase_buffer_obj_b = Utility::Open_cl::create_buffer(
        m_context, CL_MEM_READ_ONLY, cl_broadphase_work_group_size * sizeof(cl_float) * 4);
    m_broad_phase_buffer_result = Utility::Open_cl::create_buffer(m_context, CL_MEM_WRITE_ONLY,
                                                                  max_results * sizeof(cl_char));
}

Collision_strategy_open_cl::~Collision_strategy_open_cl()
{
    clReleaseKernel(m_broad_phase_inner_kernel);
    clReleaseKernel(m_broad_phase_outer_kernel);
    clReleaseProgram(m_broad_phase_program);
    clReleaseCommandQueue(m_command_queue);

    delete[] m_broad_phase_objects;
    delete[] m_broad_phase_results;

    clReleaseMemObject(m_broad_phase_buffer_obj_a);
    clReleaseMemObject(m_broad_phase_buffer_obj_b);
    clReleaseMemObject(m_broad_phase_buffer_result);

    clReleaseContext(m_context);
}

void
Collision_strategy_open_cl::find_contacts(
    const std::vector<std::shared_ptr<Physics_object>>& objects,
    std::map<Physics_object_pair, Contact_manifold>&    manifolds)
{
    size_t                                                  objects_size = objects.size();
    std::vector<Physics_object_pair>                        object_pairs;
    std::vector<std::future<std::set<Physics_object_pair>>> results;

    // inner comparisons
    for (size_t i = 0; i < objects_size; i += m_cl_broadphase_work_group_size) {
        std::vector<std::tuple<size_t, size_t>> index_pairs =
            openCL_broad_phase_inner(objects, i, m_cl_broadphase_work_group_size);
        for (const auto& pair : index_pairs) {
            object_pairs.push_back(std::make_tuple(objects[std::get<0>(pair)].get(),
                                                   objects[std::get<1>(pair)].get()));
            if (object_pairs.size() > m_collisions_per_thread) {
                results.push_back(std::async(std::launch::async,
                                             &Collision_strategy_open_cl::solve_collisions, this,
                                             std::move(object_pairs), std::ref(manifolds)));
                object_pairs.clear();
            }
        }
    }

    // outer comparisons
    int half_size = m_cl_broadphase_work_group_size >> 1;
    for (size_t i = 0; i < objects_size; i += half_size) {
        size_t j = 0;
        if (i % m_cl_broadphase_work_group_size == 0) {
            j = i + m_cl_broadphase_work_group_size;
        }
        else {
            j = i + half_size;
        }
        for (; j < objects_size; j += half_size) {
            std::vector<std::tuple<size_t, size_t>> index_pairs =
                openCL_broad_phase_outer(objects, i, j, half_size);
            for (const auto& pair : index_pairs) {
                object_pairs.push_back(std::make_tuple(objects[std::get<0>(pair)].get(),
                                                       objects[std::get<1>(pair)].get()));
                if (object_pairs.size() > m_collisions_per_thread) {
                    results.push_back(std::async(
                        std::launch::async, &Collision_strategy_open_cl::solve_collisions, this,
                        std::move(object_pairs), std::ref(manifolds)));
                    object_pairs.clear();
                }
            }
        }
    }
    // If object_pairs is not empty then there are some left over
    // pairs that need to be run through the collision solver
    if (!object_pairs.empty()) {
        results.push_back(std::async(std::launch::async,
                                     &Collision_strategy_open_cl::solve_collisions, this,
                                     std::move(object_pairs), std::ref(manifolds)));
    }

    // collect results from threads
    std::set<Physics_object_pair> colliding_pairs;
    for (auto& result : results) {
        const auto& result_set = result.get();
        colliding_pairs.insert(result_set.begin(), result_set.end());
    }

    // remove any stale contacts
    for (auto iter = manifolds.begin(), end = manifolds.end(); iter != end;) {
        if (colliding_pairs.find(iter->first) == colliding_pairs.end()) {
            manifolds.erase(iter++);
        }
        else {
            ++iter;
        }
    }
}

std::vector<std::tuple<size_t, size_t>>
Collision_strategy_open_cl::openCL_broad_phase_inner(
    const std::vector<std::shared_ptr<Physics_object>>& objects, size_t offset, size_t length)
{
    if (offset + length > objects.size()) {
        length = objects.size() - offset;
    }
    for (size_t i = 0; i < length; ++i) {
        std::shared_ptr<Physics_object> a = objects[i + offset];
        m_broad_phase_objects[i * 4 + 0]  = a->coordinate_space().position().x();
        m_broad_phase_objects[i * 4 + 1]  = a->coordinate_space().position().y();
        m_broad_phase_objects[i * 4 + 2]  = a->coordinate_space().position().z();
        m_broad_phase_objects[i * 4 + 3]  = a->model().radius();
    }
    size_t comparison_count = static_cast<int>(length * (length - 1) / 2.0f);

    cl_int num_elements = length;
    Utility::Open_cl::set_kernel_arg(m_broad_phase_inner_kernel, 0, sizeof(cl_mem),
                                     &m_broad_phase_buffer_obj_a);
    Utility::Open_cl::set_kernel_arg(m_broad_phase_inner_kernel, 1, sizeof(cl_int), &num_elements);
    Utility::Open_cl::set_kernel_arg(m_broad_phase_inner_kernel, 2, sizeof(cl_mem),
                                     &m_broad_phase_buffer_result);

    Utility::Open_cl::enqueue_write_buffer(m_command_queue, m_broad_phase_buffer_obj_a, CL_FALSE,
                                           4 * sizeof(cl_float) * length, m_broad_phase_objects);
    Utility::Open_cl::enqueue_nd_range_kernel(m_command_queue, m_broad_phase_inner_kernel,
                                              &comparison_count, nullptr);
    Utility::Open_cl::enqueue_read_buffer(m_command_queue, m_broad_phase_buffer_result, CL_TRUE,
                                          comparison_count * sizeof(cl_char),
                                          m_broad_phase_results);

    std::vector<std::tuple<size_t, size_t>> result_vector;
    int                                     global_index = 0;
    for (size_t i = 0; i < length; ++i) {
        for (size_t j = i + 1; j < length; ++j) {
            if (m_broad_phase_results[global_index++] == 1) {
                result_vector.push_back(std::make_tuple(i + offset, j + offset));
            }
        }
    }

    return result_vector;
}

std::vector<std::tuple<size_t, size_t>>
Collision_strategy_open_cl::openCL_broad_phase_outer(
    const std::vector<std::shared_ptr<Physics_object>>& objects, size_t offset_a, size_t offset_b,
    size_t length)
{
    size_t length_b = 0;
    for (size_t i = 0; i < length; ++i) {
        std::shared_ptr<Physics_object> a = objects[i + offset_a];
        m_broad_phase_objects[i * 4 + 0]  = a->coordinate_space().position().x();
        m_broad_phase_objects[i * 4 + 1]  = a->coordinate_space().position().y();
        m_broad_phase_objects[i * 4 + 2]  = a->coordinate_space().position().z();
        m_broad_phase_objects[i * 4 + 3]  = a->model().radius();
        if (i + offset_b >= objects.size()) {
            continue;
        }
        a                                           = objects[i + offset_b];
        m_broad_phase_objects[(i + length) * 4 + 0] = a->coordinate_space().position().x();
        m_broad_phase_objects[(i + length) * 4 + 1] = a->coordinate_space().position().y();
        m_broad_phase_objects[(i + length) * 4 + 2] = a->coordinate_space().position().z();
        m_broad_phase_objects[(i + length) * 4 + 3] = a->model().radius();
        ++length_b;
    }
    size_t comparison_count = length * length_b;

    cl_int num_elements = length;
    Utility::Open_cl::set_kernel_arg(m_broad_phase_outer_kernel, 0, sizeof(cl_mem),
                                     &m_broad_phase_buffer_obj_a);
    Utility::Open_cl::set_kernel_arg(m_broad_phase_outer_kernel, 1, sizeof(cl_mem),
                                     &m_broad_phase_buffer_obj_b);
    Utility::Open_cl::set_kernel_arg(m_broad_phase_outer_kernel, 2, sizeof(cl_int), &length_b);
    Utility::Open_cl::set_kernel_arg(m_broad_phase_outer_kernel, 3, sizeof(cl_mem),
                                     &m_broad_phase_buffer_result);

    Utility::Open_cl::enqueue_write_buffer(m_command_queue, m_broad_phase_buffer_obj_a, CL_FALSE,
                                           4 * sizeof(cl_float) * length, m_broad_phase_objects);
    Utility::Open_cl::enqueue_write_buffer(m_command_queue, m_broad_phase_buffer_obj_b, CL_FALSE,
                                           4 * sizeof(cl_float) * length_b,
                                           &m_broad_phase_objects[length * 4]);
    Utility::Open_cl::enqueue_nd_range_kernel(m_command_queue, m_broad_phase_outer_kernel,
                                              &comparison_count, nullptr);
    Utility::Open_cl::enqueue_read_buffer(m_command_queue, m_broad_phase_buffer_result, CL_TRUE,
                                          comparison_count * sizeof(cl_char),
                                          m_broad_phase_results);

    std::vector<std::tuple<size_t, size_t>> result_vector;
    int                                     global_index = 0;
    for (size_t i = 0; i < length; ++i) {
        for (size_t j = 0; j < length_b; ++j) {
            if (m_broad_phase_results[global_index++] == 1) {
                result_vector.push_back(std::make_tuple(i + offset_a, j + offset_b));
            }
        }
    }

    return result_vector;
}

std::set<Collision_strategy::Physics_object_pair>
Collision_strategy_open_cl::solve_collisions(
    std::vector<Physics_object_pair>&&               inputs,
    std::map<Physics_object_pair, Contact_manifold>& manifolds)
{
    std::set<Physics_object_pair> new_pairs;
    for (const auto& object_tuple : inputs) {
        std::vector<Contact_manifold::Contact> contacts;
        if (m_collision_solver.intersection(*std::get<0>(object_tuple), *std::get<1>(object_tuple),
                                            contacts)) {
            auto contact_manifold = manifolds.find(object_tuple);
            if (contact_manifold == manifolds.end()) {
                std::unique_lock<std::mutex> lock(m_manifolds_mutex);
                contact_manifold =
                    manifolds
                        .insert(std::make_pair(
                            object_tuple,
                            Contact_manifold(*std::get<0>(object_tuple), *std::get<1>(object_tuple),
                                             m_manifold_persistent_threshold,
                                             m_manifold_movement_threshold)))
                        .first;
            }
            contact_manifold->second.prune_old_contacts();
            contact_manifold->second.insert(contacts);
            new_pairs.insert(object_tuple);
        }
    }
    return new_pairs;
}

}  // namespace Physics
}  // namespace Dubious
