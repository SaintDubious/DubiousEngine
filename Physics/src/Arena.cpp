#include "Arena.h"
#include "Physics_object.h"
#include "Physics_model.h"
#include "Collision_strategy_simple.h"
#include "Collision_strategy_multi_threaded.h"
#include "Collision_strategy_open_cl.h"

#include <set>
#include <iostream>
#include <future>

namespace Dubious {
namespace Physics {

Arena::Arena(const Settings& settings)
    : m_constraint_solver(settings.constraint.step_size, settings.constraint.beta,
                          settings.constraint.coefficient_of_restitution, settings.constraint.slop)
    , m_settings(settings)
{
    switch (m_settings.collision.strategy) {
    case Collision_solver_settings::Strategy::SINGLE_THREADED:
        m_collision_strategy = std::make_unique<Collision_strategy_simple>(
            m_settings.collision.manifold_persistent_threshold);
        break;
    case Collision_solver_settings::Strategy::MULTI_THREADED:
        m_collision_strategy = std::make_unique<Collision_strategy_multi_threaded>(
            m_settings.collision.manifold_persistent_threshold,
            m_settings.collision.mt_collisions_work_group_size);
        break;
    case Collision_solver_settings::Strategy::OPENCL:
        m_collision_strategy = std::make_unique<Collision_strategy_open_cl>(
            m_settings.collision.manifold_persistent_threshold,
            m_settings.collision.cl_collisions_per_thread,
            m_settings.collision.cl_collisions_work_group_size);
        break;
    default:
        throw std::runtime_error("Unknown collision strategy requested");
    }
}

void
Arena::push_back(std::shared_ptr<Physics_object> obj)
{
    m_objects.push_back(obj);
}

void
Arena::run_physics(float elapsed)
{
    m_elapsed += elapsed;
    while (m_elapsed > m_settings.constraint.step_size) {
        for (const auto& o : m_objects) {
            o->velocity() =
                o->velocity() + (o->force() * o->inverse_mass()) * m_settings.constraint.step_size;
            o->angular_velocity() =
                o->angular_velocity() +
                (o->torque() * o->inverse_moment_of_inertia()) * m_settings.constraint.step_size;
        }

        m_collision_strategy->find_contacts(m_objects, m_manifolds);

        for (auto& manifold : m_manifolds) {
            Physics_object* a = std::get<0>(manifold.first);
            Physics_object* b = std::get<1>(manifold.first);
            m_constraint_solver.warm_start(*a, *b, manifold.second);
        }

        for (int i = 0; i < m_settings.constraint.iterations; ++i) {
            for (auto& manifold : m_manifolds) {
                Physics_object* a = std::get<0>(manifold.first);
                Physics_object* b = std::get<1>(manifold.first);
                m_constraint_solver.solve(*a, *b, manifold.second);
            }
        }

        for (const auto& o : m_objects) {
            o->coordinate_space().position() =
                o->coordinate_space().position() + o->velocity() * m_settings.constraint.step_size;
            o->coordinate_space().rotation() =
                o->coordinate_space().rotation() +
                (o->angular_velocity() * o->coordinate_space().rotation() * 0.5) *
                    m_settings.constraint.step_size;
            o->coordinate_space().rotation().normalize();
        }

        m_elapsed -= m_settings.constraint.step_size;
    }
}

}  // namespace Physics
}  // namespace Dubious
