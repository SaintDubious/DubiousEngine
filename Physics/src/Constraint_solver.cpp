#include "Constraint_solver.h"
#include "Physics_object.h"

#include <Vector_math.h>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

namespace {

// The lagrangian multiplier (denoted by lambda) is a scalar derived from a heck of a
// lot of really big matrices. I'm trying my hardest to not have to create a matrix
// class for this thing, so I went ahead and unwrapped it all into a bunch of vector
// dot products.
float lagrangian_multiplier( const Math::Vector& n, const Math::Vector& r_a, const Math::Vector& r_b,
                             const Math::Vector& v_a, const Math::Vector& av_a, 
                             float mass_a, float inertial_tensor_a,
                             const Math::Vector& v_b, const Math::Vector& av_b,
                             float mass_b, float inertial_tensor_b )
{
    float j_dot_v = Math::dot_product((-1*n), v_a) + Math::dot_product(Math::cross_product(-1*r_a,n), av_a) +
                    Math::dot_product(n, v_b)      + Math::dot_product(Math::cross_product(r_b,n),av_b);
    float inverse_m_a = 1.0f/mass_a;
    float inverse_m_b = 1.0f/mass_b;
    float inverse_tensor_a = 1.0f/inertial_tensor_a;
    float inverse_tensor_b = 1.0f/inertial_tensor_b;

    float denom_a = 0;
    if (!Physics_object::is_stationary(mass_a)) {
        Math::Vector negra_x_n = Math::cross_product( -1*r_a, n );
        denom_a = Math::dot_product( -1*n*inverse_m_a, -1*n ) + Math::dot_product( negra_x_n*inverse_tensor_a, negra_x_n);
    }
    float denom_b = 0;
    if (!Physics_object::is_stationary(mass_b)) {
        Math::Vector rb_x_n = Math::cross_product( r_b, n );
        denom_b = Math::dot_product(n*inverse_m_b, n) + Math::dot_product( rb_x_n*inverse_tensor_b, rb_x_n);
    }
    float denom = denom_a + denom_b;
    float lambda = -j_dot_v / denom;

    return lambda;
}  

Constraint_solver::Velocity_matrix delta_v( float lambda, const Math::Vector& n, 
                                            const Math::Vector& r_a, const Math::Vector& r_b,
                                            float mass_a, float inertial_tensor_a,
                                            float mass_b, float inertial_tensor_b )
{
    Constraint_solver::Velocity_matrix result;
    if (!Physics_object::is_stationary(mass_a)) {
        float inverse_m_a = 1.0f/mass_a;
        float inverse_tensor_a = 1.0f/inertial_tensor_a;
        result.a_linear  = -1*n * inverse_m_a * lambda;
        result.a_angular = inverse_tensor_a * (Math::cross_product( -1* r_a, n)) * lambda;
    }
    if (!Physics_object::is_stationary(mass_b)) {
        float inverse_m_b = 1.0f/mass_b;
        float inverse_tensor_b = 1.0f/inertial_tensor_b;
        result.b_linear  = n * inverse_m_b * lambda;
        result.b_angular = inverse_tensor_b * (Math::cross_product( r_b, n)) * lambda;
    }

    return result;
}
}
                                
Constraint_solver::Velocity_matrix Constraint_solver::solve( const Physics_object& a, const Physics_object& b, std::vector<Collision_solver::Contact>& contacts )
{
    Constraint_solver::Velocity_matrix result;
    result.a_linear  = a.velocity();
    result.a_angular = a.angular_velocity();
    result.b_linear  = b.velocity();
    result.b_angular = b.angular_velocity();

    for (const auto& c : contacts) {
        Math::Vector r_a = c.contact_point_a - a.coordinate_space().position();
        Math::Vector r_b = c.contact_point_b - b.coordinate_space().position();



        Constraint_solver::Velocity_matrix delta = delta_v( lagrangian_multiplier( c.normal, r_a, r_b, 
                                    a.velocity(), a.angular_velocity(), a.mass(), a.moment_of_inertia(),
                                    b.velocity(), b.angular_velocity(), b.mass(), b.moment_of_inertia()), 
                                    c.normal, r_a, r_b, a.mass(), a.moment_of_inertia(), b.mass(), b.moment_of_inertia() );

            


        result.a_linear  = result.a_linear  + delta.a_linear;
        result.a_angular = result.a_angular + delta.a_angular;
        result.b_linear  = result.b_linear  + delta.b_linear;
        result.b_angular = result.b_angular + delta.b_angular;
    }



    return result;
}

}}
