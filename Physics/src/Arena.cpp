#include "Arena.h"
#include "Physics_object.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

Arena::Arena( float step_size )
    : m_integrator( step_size )
    , m_constraint_solver( step_size )
    , m_step_size( step_size )
{
}

void Arena::push_back( std::shared_ptr<Physics_object> obj )
{
    m_objects.push_back( obj );
}

void Arena::run_physics( float elapsed )
{
//    m_integrator.update( m_objects, elapsed );

    m_elapsed += elapsed;
    while (m_elapsed > m_step_size) {

        for (const auto& o : m_objects) {
            if (!Physics_object::is_stationary(o->mass())) {
                o->velocity() = o->velocity() + (o->force()/o->mass())*m_step_size;
                o->angular_velocity() = o->angular_velocity() + (o->torque()/o->moment_of_inertia())*m_step_size;
            }
        }
        

        for (size_t i=0; i<m_objects.size(); ++i) {
            std::shared_ptr<Physics_object> a = m_objects[i];
            for (size_t j=i+1; j<m_objects.size(); ++j) {
                std::shared_ptr<Physics_object> b = m_objects[j];
                std::vector<Contact_manifold::Contact> contacts;
                if (m_collision_solver.intersection( *a, *b, contacts )) {

                    auto contact_manifold = m_manifolds.find( std::make_tuple(a,b) );
                    if (contact_manifold == m_manifolds.end()) {
                        contact_manifold = m_manifolds.insert( std::make_pair(std::make_tuple(a,b), Contact_manifold( a, b )) ).first;
                    }
                    contact_manifold->second.prune_old_contacts();
                    contact_manifold->second.insert( contacts );

                    Constraint_solver::Velocity_matrix velocities;
                    velocities = m_constraint_solver.solve( *a, *b, contact_manifold->second );

                    a->velocity()           = velocities.a_linear;
                    a->angular_velocity()   = velocities.a_angular;
                    b->velocity()           = velocities.b_linear;
                    b->angular_velocity()   = velocities.b_angular;
                }
                else {
                    m_manifolds.erase( std::make_tuple(a,b) );
                }
            }
        }

        for (const auto& o : m_objects) {
            if (!Physics_object::is_stationary(o->mass())) {
                o->coordinate_space().position() = o->coordinate_space().position() + o->velocity()*m_step_size;
                Math::Quaternion q_angular_veloticy = o->angular_velocity() * o->coordinate_space().rotation() * 0.5;
                o->coordinate_space().rotation() = o->coordinate_space().rotation() + q_angular_veloticy*m_step_size;
                o->coordinate_space().rotation().normalize();
            }
        }

        m_elapsed -= m_step_size;
    }

}

}}
