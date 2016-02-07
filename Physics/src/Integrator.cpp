#include "Integrator.h"
#include "Physics_object.h"

#include <Vector.h>
#include <Point.h>

#include <tuple>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

//////////////////////////////////////////////////////////////
Integrator::Integrator( float step_size )
    : m_step_size( step_size )
{
}

namespace {

typedef std::tuple<Math::Vector, Math::Vector> Vector_tuple;

//////////////////////////////////////////////////////////////
Vector_tuple evaluate_rk4(const Vector_tuple &start, const Math::Vector& acceleration, float dt, const Vector_tuple &derivative)
{
	Vector_tuple state;
	std::get<0>(state) = std::get<0>(start) + std::get<0>(derivative) * dt;
	std::get<1>(state) = std::get<1>(start) + std::get<1>(derivative) * dt;

	// Acceleration is constant over the time step because 
	// the force has only been applied during the control phase.
	return std::make_tuple( std::get<1>(state), acceleration );
}

//////////////////////////////////////////////////////////////
// Perfrom RK4 integration.  See:
// http://gafferongames.com/game-physics/integration-basics/
Vector_tuple integrate_rk4( const Math::Point& position, const Math::Vector& velocity, const Math::Vector& acceleration, float elapsed )
{
	Vector_tuple state( position-Math::Point(), velocity );

	Vector_tuple a = evaluate_rk4( state, acceleration, 0, std::make_tuple(Math::Vector(),Math::Vector()) );
	Vector_tuple b = evaluate_rk4( state, acceleration, elapsed*0.5f, a );
	Vector_tuple c = evaluate_rk4( state, acceleration, elapsed*0.5f, b );
	Vector_tuple d = evaluate_rk4( state, acceleration, elapsed, c );

	Math::Vector dx_dt = (std::get<0>(a) + (std::get<0>(b) + std::get<0>(c))*2.0f + std::get<0>(d)) * (1.0f/6.0f);
	Math::Vector dv_dt = (std::get<1>(a) + (std::get<1>(b) + std::get<1>(c))*2.0f + std::get<1>(d)) * (1.0f/6.0f);

	std::get<0>(state) = std::get<0>(state) + (dx_dt * elapsed);
	std::get<1>(state) = std::get<1>(state) + (dv_dt * elapsed);

	return state;
}

//////////////////////////////////////////////////////////////
Vector_tuple integrate_linear( const Physics_object& physics_object, float elapsed )
{
	return integrate_rk4( physics_object.coordinate_space().position(), 
                         physics_object.velocity(), 
                         physics_object.force() * (1.0f/physics_object.mass()), elapsed );
}

}

//////////////////////////////////////////////////////////////
void Integrator::update( std::list<std::shared_ptr<Physics_object>>& objects, float elapsed )
{
    m_elapsed += elapsed;
    while (m_elapsed > m_step_size) {
        for (auto& object : objects) {
            Math::Vector new_position;
            Math::Vector new_velocity;
            std::tie(new_position, new_velocity) = integrate_linear( *object, m_step_size );
            object->coordinate_space().position() = Math::Point() + new_position;
            object->velocity() = new_velocity;
        }

        m_elapsed -= m_step_size;
    }
}



}}
