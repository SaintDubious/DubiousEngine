#include "Arena.h"
#include "Physics_object.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

Arena::Arena( float step_size )
    : m_integrator( step_size )
    , m_step_size( step_size )
{
}

void Arena::push_back( std::shared_ptr<Physics_object> obj )
{
    m_objects.push_back( obj );
}

void Arena::run_physics( float elapsed )
{
    m_integrator.update( m_objects, elapsed );

    for (size_t i=0; i<m_objects.size(); ++i) {
        std::shared_ptr<Physics_object> a = m_objects[i];
        for (size_t j=i+1; j<m_objects.size(); ++j) {
            std::shared_ptr<Physics_object> b = m_objects[j];
            std::vector<Collision_solver::Contact> contacts;
            if (m_collision_solver.intersection( *a, *b, contacts )) {
                Constraint_solver::Velocity_matrix velocities;
                velocities = m_constraint_solver.solve( *a, *b, contacts );

                a->velocity()           = velocities.a_linear;
                a->angular_velocity()   = velocities.a_angular;
                b->velocity()           = velocities.b_linear;
                b->angular_velocity()   = velocities.b_angular;
            }
        }
    }
}

}}
