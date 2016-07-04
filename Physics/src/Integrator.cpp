#include "Integrator.h"
#include "Physics_object.h"

#include <Vector.h>
#include <Vector_math.h>
#include <Point.h>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

//////////////////////////////////////////////////////////////
Integrator::Integrator( float step_size )
    : m_step_size( step_size )
{
}

namespace {

//////////////////////////////////////////////////////////////
class Linear_state {
public:
    Linear_state( const Math::Vector& position, const Math::Vector& momentum, float mass )
        : m_position( position )
        , m_momentum( momentum )
        , m_mass( mass )
    {
        m_velocity = m_momentum/m_mass;
    }

    const Math::Vector& position() const { return m_position; }
    const Math::Vector& momentum() const { return m_momentum; }
    const Math::Vector& velocity() const { return m_velocity; }
    float               mass() const { return m_mass; }

private:
    Math::Vector    m_position;
    Math::Vector    m_momentum;
    Math::Vector    m_velocity;
    float           m_mass;
};

//////////////////////////////////////////////////////////////
struct Linear_derivative {
    Math::Vector    m_velocity;
    Math::Vector    m_force;
};

//////////////////////////////////////////////////////////////
Linear_derivative evaluate_linear(const Linear_state& initial, float dt, const Linear_derivative& derivative )
{
    Linear_state state(
        initial.position() + derivative.m_velocity * dt,
        initial.momentum() + derivative.m_force * dt,
        initial.mass()
    );

    Linear_derivative output;
    output.m_velocity = state.velocity();
    output.m_force    = derivative.m_force;

    return output;
}

//////////////////////////////////////////////////////////////
void linear_rk4( Linear_state& state, Math::Vector& force, float dt )
{
    Linear_derivative a, b, c, d;

    Linear_derivative start_derivative;
    start_derivative.m_force = force;
    a = evaluate_linear( state, 0.0f, start_derivative );
    b = evaluate_linear( state, dt*0.5f, a );
    c = evaluate_linear( state, dt*0.5f, b );
    d = evaluate_linear( state, dt, c );

    Math::Vector dxdt = (a.m_velocity + (b.m_velocity + c.m_velocity)*2.0f + d.m_velocity) * (1.0f / 6.0f);
    Math::Vector dpdt = (a.m_force +    (b.m_force +    c.m_force)*2.0f +    d.m_force)    * (1.0f / 6.0f);

    state = Linear_state( 
        state.position() + dxdt*dt,
        state.momentum() + dpdt*dt,
        state.mass()
    );
}

//////////////////////////////////////////////////////////////
class Angular_state {
public:
    Angular_state( const Math::Quaternion& orientation, const Math::Vector& momentum, float moment_of_inertia )
        : m_orientation( orientation )
        , m_momentum( momentum )
        , m_moment_of_inertia( moment_of_inertia )
    {
        m_velocity = m_momentum * (1.0f / moment_of_inertia );
        // derivative of orientation quaternion is angular
        // velocity expressed as quaternion
        m_q_velocity = m_velocity * m_orientation * 0.5;
    }

    const Math::Quaternion& orientation() const { return m_orientation; }
    const Math::Vector&     momentum() const { return m_momentum; }
    const Math::Vector&     velocity() const { return m_velocity; }
    const Math::Quaternion& q_velocity() const { return m_q_velocity; }
    float                   moment_of_inertia() const { return m_moment_of_inertia; }

private:
    Math::Quaternion    m_orientation;
    Math::Vector        m_momentum;
    Math::Vector        m_velocity;
    Math::Quaternion    m_q_velocity;
    float               m_moment_of_inertia;
};

//////////////////////////////////////////////////////////////
struct Angular_derivative {
    Math::Quaternion    m_q_velocity;
    Math::Vector        m_torque;
};

//////////////////////////////////////////////////////////////
Angular_derivative evaluate_angular(const Angular_state& initial, float dt, const Angular_derivative& derivative )
{
    Angular_state state(
        initial.orientation() + derivative.m_q_velocity * dt,
        initial.momentum()    + derivative.m_torque   * dt,
        initial.moment_of_inertia()
    );

    Angular_derivative output;
    output.m_q_velocity = state.q_velocity();
    output.m_torque   = derivative.m_torque;

    return output;
}

//////////////////////////////////////////////////////////////
void angular_rk4( Angular_state& state, Math::Vector& torque, float dt )
{
    Angular_derivative a, b, c, d;

    Angular_derivative start_derivative;
    start_derivative.m_torque = torque;
    a = evaluate_angular( state, 0.0f, start_derivative );
    b = evaluate_angular( state, dt*0.5f, a );
    c = evaluate_angular( state, dt*0.5f, b );
    d = evaluate_angular( state, dt, c );

    Math::Quaternion dxdt = (a.m_q_velocity + (b.m_q_velocity + c.m_q_velocity)*2.0f + d.m_q_velocity) * (1.0f / 6.0f);
    Math::Vector     dpdt = (a.m_torque +   (b.m_torque +   c.m_torque)*2.0f +   d.m_torque)   * (1.0f / 6.0f);

    state = Angular_state( 
        state.orientation() + dxdt*dt,
        state.momentum() + dpdt*dt,
        state.moment_of_inertia()
    );
}

}

//////////////////////////////////////////////////////////////
void Integrator::update( std::list<std::shared_ptr<Physics_object>>& objects, float elapsed )
{
    m_elapsed += elapsed;
    while (m_elapsed > m_step_size) {
        for (auto& object : objects) {
            integrate_linear( *object, m_step_size );
            integrate_angular( *object, m_step_size );
        }

        m_elapsed -= m_step_size;
    }
}

//////////////////////////////////////////////////////////////
void Integrator::integrate_linear( Physics_object& physics_object, float dt ) const
{
    Linear_state state( Math::to_vector(physics_object.coordinate_space().position()),
                        physics_object.velocity()*physics_object.mass(),
                        physics_object.mass() );
	linear_rk4( state, physics_object.force(), dt );
    physics_object.coordinate_space().position() = Math::to_point(state.position());
    physics_object.velocity() = state.velocity();
}

//////////////////////////////////////////////////////////////
void Integrator::integrate_angular( Physics_object& physics_object, float dt ) const
{
    Angular_state state( physics_object.coordinate_space().rotation(),
                        physics_object.angular_velocity()*physics_object.moment_of_inertia(),
                        physics_object.moment_of_inertia() );
	angular_rk4( state, physics_object.torque(), dt );
    Math::Quaternion orientation = state.orientation();
    Math::Vector     velocity    = state.velocity();
    orientation.normalize();
    physics_object.coordinate_space().rotation() = orientation;
    physics_object.angular_velocity() = velocity;
}

}}
