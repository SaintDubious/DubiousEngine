#include "Constraint_strategy_open_cl.h"

#include "Constraint_solver.cl"

#include <iostream>

namespace Dubious {
namespace Physics {

Constraint_strategy_open_cl::Constraint_strategy_open_cl(float time_step, float beta, float cor,
                                                         float slop)
{
    bool opencl_available;
    std::tie(opencl_available, m_platform_id, m_device_id) = Utility::Open_cl::setup();
    if (!opencl_available) {
        throw std::runtime_error("Attempted to use OpenCL, but it is not supported");
    }
    m_context       = Utility::Open_cl::create_context(m_platform_id, m_device_id);
    m_command_queue = Utility::Open_cl::create_command_queue(m_context, m_device_id);
    m_program       = Utility::Open_cl::create_program(constraint_solver, m_context, m_device_id);
    m_warm_start_kernel = Utility::Open_cl::create_kernel(m_program, "warm_start");
    m_solve_kernel      = Utility::Open_cl::create_kernel(m_program, "solve");

    size_t ret_size;
    size_t work_group_size;
    cl_int rc;
    rc = clGetKernelWorkGroupInfo(m_warm_start_kernel, m_device_id, CL_KERNEL_WORK_GROUP_SIZE,
                                  sizeof(size_t), &work_group_size, &ret_size);
    if (CL_SUCCESS != rc) {
        throw std::runtime_error("clGetKernelWorkGroupInfo");
    }
    std::cout << "CL_KERNEL_WORK_GROUP_SIZE(m_warm_start_kernel): " << work_group_size << "\n";

    rc = clGetKernelWorkGroupInfo(m_solve_kernel, m_device_id, CL_KERNEL_WORK_GROUP_SIZE,
                                  sizeof(size_t), &work_group_size, &ret_size);
    if (CL_SUCCESS != rc) {
        throw std::runtime_error("clGetKernelWorkGroupInfo");
    }
    std::cout << "CL_KERNEL_WORK_GROUP_SIZE(m_solve_kernel): " << work_group_size << "\n";
}

Constraint_strategy_open_cl::~Constraint_strategy_open_cl()
{
    clReleaseKernel(m_solve_kernel);
    clReleaseKernel(m_warm_start_kernel);
    clReleaseProgram(m_program);
    clReleaseCommandQueue(m_command_queue);

    clReleaseContext(m_context);
}

void
Constraint_strategy_open_cl::warm_start(std::map<Physics_object_pair, Contact_manifold>& manifolds)
{
    for (auto& manifold : manifolds) {
    }
}

void
Constraint_strategy_open_cl::solve(int                                              iterations,
                                   std::map<Physics_object_pair, Contact_manifold>& manifolds)
{
    for (int i = 0; i < iterations; ++i) {
        for (auto& manifold : manifolds) {
        }
    }
}

}  // namespace Physics
}  // namespace Dubious
