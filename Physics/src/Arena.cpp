#include "Arena.h"
#include "Physics_object.h"
#include "Physics_model.h"
#include "Collision_strategy_simple.h"
#include "Collision_strategy_multi_threaded.h"
#include "Collision_strategy_open_cl.h"
#include "Constraint_strategy_simple.h"
#include "Constraint_strategy_multi_threaded.h"

#include <set>
#include <iostream>
#include <future>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

Arena::Arena( const Settings& settings )
    : m_settings( settings )
{
    switch (m_settings.collision_strategy) {
    case Settings::Collision_strategy::SINGLE_THREADED:
        m_collision_strategy = std::make_unique<Collision_strategy_simple>( m_settings.manifold_persistent_threshold );
        break;
    case Settings::Collision_strategy::MULTI_THREADED:
        m_collision_strategy = std::make_unique<Collision_strategy_multi_threaded>( m_settings.manifold_persistent_threshold, m_settings.mt_collisions_work_group_size );
        break;
    case Settings::Collision_strategy::OPENCL:
        m_collision_strategy = std::make_unique<Collision_strategy_open_cl>( m_settings.manifold_persistent_threshold, m_settings.collisions_per_thread, m_settings.cl_collisions_work_group_size );
        break;
    default:
        throw std::runtime_error( "Unknown collision strategy requested" );
    }
    m_constraint_strategy = std::make_unique<Constraint_strategy_simple>(m_settings.step_size, 
                           m_settings.beta, 
                           m_settings.coefficient_of_restitution, 
                           m_settings.slop);
}

//////////////////////////////////////////////////////////////
void Arena::push_back( std::shared_ptr<Physics_object> obj )
{
    m_objects.push_back( obj );
}

//////////////////////////////////////////////////////////////
void Arena::run_physics( float elapsed )
{
    m_elapsed += elapsed;
    while (m_elapsed > m_settings.step_size) {

        for (const auto& o : m_objects) {
            o->velocity() = o->velocity() + (o->force()*o->inverse_mass())*m_settings.step_size;
            o->angular_velocity() = o->angular_velocity() + (o->torque()*o->inverse_moment_of_inertia())*m_settings.step_size;
        }

        m_collision_strategy->find_contacts( m_objects, m_manifolds );
        
        m_constraint_strategy->warm_start( m_manifolds );
        m_constraint_strategy->solve( m_settings.iterations, m_manifolds );

        for (const auto& o : m_objects) {
            o->coordinate_space().position() = o->coordinate_space().position() + o->velocity()*m_settings.step_size;
            o->coordinate_space().rotation() = o->coordinate_space().rotation() + (o->angular_velocity() * o->coordinate_space().rotation() * 0.5)*m_settings.step_size;
            o->coordinate_space().rotation().normalize();
        }

        m_elapsed -= m_settings.step_size;
    }
}

}}
