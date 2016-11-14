#include "Arena.h"
#include "Physics_object.h"

#include <set>
#include <iostream>
#include <future>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

Arena::Arena( const Settings& settings )
    : m_constraint_solver( settings.step_size, 
                           settings.beta, 
                           settings.coefficient_of_restitution, 
                           settings.slop )
    , m_step_size( settings.step_size )
    , m_velocity_iterations( settings.iterations )
    , m_settings( settings )
{
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
    while (m_elapsed > m_step_size) {
        for (const auto& o : m_objects) {
            o->velocity() = o->velocity() + (o->force()*o->inverse_mass())*m_step_size;
            o->angular_velocity() = o->angular_velocity() + (o->torque()*o->inverse_moment_of_inertia())*m_step_size;
        }

        std::set<std::tuple<std::shared_ptr<Physics_object>,std::shared_ptr<Physics_object>>> new_pairs;
        for (size_t i=0; i<m_objects.size(); ++i) {
            std::shared_ptr<Physics_object> a = m_objects[i];
            for (size_t j=i+1; j<m_objects.size(); ++j) {
                std::shared_ptr<Physics_object> b = m_objects[j];
                if (!m_collision_solver.broad_phase_intersection( *a, *b )) {
                    continue;
                }
                std::vector<Contact_manifold::Contact> contacts;
                if (m_collision_solver.intersection( *a, *b, contacts )) {
                    auto contact_manifold = m_manifolds.find( std::make_tuple(a,b) );
                    if (contact_manifold == m_manifolds.end()) {
                        contact_manifold = m_manifolds.insert( std::make_pair(std::make_tuple(a,b), 
                            Contact_manifold( a, b, m_settings.manifold_persistent_threshold )) ).first;
                    }
                    contact_manifold->second.prune_old_contacts();
                    contact_manifold->second.insert( contacts );
                    new_pairs.insert( std::make_tuple(a,b) );
                }
            }
        }

        // remove any stale contacts
        for (auto iter=m_manifolds.begin(), end=m_manifolds.end(); iter!=end;) {
            if (new_pairs.find(iter->first) == new_pairs.end()) {
                iter = m_manifolds.erase( iter );
            }
            else {
                ++iter;
            }
        }
        
        for (auto &manifold : m_manifolds) {
            std::shared_ptr<Physics_object> a = std::get<0>(manifold.first);
            std::shared_ptr<Physics_object> b = std::get<1>(manifold.first);
            m_constraint_solver.warm_start( *a, *b, manifold.second );
        }
        for (int i=0; i<m_velocity_iterations; ++i) {
            for (auto &manifold : m_manifolds) {
                std::shared_ptr<Physics_object> a = std::get<0>(manifold.first);
                std::shared_ptr<Physics_object> b = std::get<1>(manifold.first);
                m_constraint_solver.solve( *a, *b, manifold.second );
            }
        }

        for (const auto& o : m_objects) {
            o->coordinate_space().position() = o->coordinate_space().position() + o->velocity()*m_step_size;
            Math::Quaternion q_angular_veloticy = o->angular_velocity() * o->coordinate_space().rotation() * 0.5;
            o->coordinate_space().rotation() = o->coordinate_space().rotation() + q_angular_veloticy*m_step_size;
            o->coordinate_space().rotation().normalize();
        }

        m_elapsed -= m_step_size;
    }
}

}}
