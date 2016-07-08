#include "Constraint_solver.h"
#include "Physics_object.h"

#include <Vector_math.h>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

Constraint_solver::Constraint_solver( float time_step )
    : m_time_step( time_step )
{
}

namespace {

float baumgarte_term( float dt, const Math::Vector& n, const Math::Point& p_a, const Math::Point& p_b )
{
    // BETA is the number you can play with to try different things. In my simple 
    // test with stacked blocks, when it was 0.01 they would very slowly penetrate.
    // When it was 0.9 they hopped up and down until the stack fell.
    const float BETA = 0.1f;
    float d = Math::dot_product( (p_b - p_a), n );
    // According to Allen Chou's web page, the Baumgarte term should be negative.
    // However when I use negative, my colliders end up sucking into each other
    // and generally going haywire. 
    return (BETA/dt) * d;
}

float restitution_term( const Math::Vector& n, 
                        const Math::Vector& r_a, const Math::Vector& r_b,
                        const Math::Vector& v_a, const Math::Vector& av_a, 
                        const Math::Vector& v_b, const Math::Vector& av_b)
{
    const float COEFFICIENT_OF_RESTITUTION = 0.2f;
    Math::Vector a_part = -1*v_a - Math::cross_product( av_a, r_a );
    Math::Vector b_part =    v_b + Math::cross_product( av_b, r_b );

    return Math::dot_product( (a_part+b_part) * COEFFICIENT_OF_RESTITUTION, n );
}

// The lagrangian multiplier (denoted by lambda) is a scalar derived from a heck of a
// lot of really big matrices. I'm trying my hardest to not have to create a matrix
// class for this thing, so I went ahead and unwrapped it all into a bunch of vector
// dot products.
float lagrangian_multiplier( float dt, const Math::Vector& n, 
                             const Math::Point& p_a, const Math::Point& p_b,
                             const Math::Vector& r_a, const Math::Vector& r_b,
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
    float baumgarte = baumgarte_term( dt, n, p_a, p_b ); 
    float restitution = restitution_term( n, r_a, r_b, v_a, av_a, v_b, av_b );
    float lambda = -(j_dot_v + baumgarte + restitution)  / denom;

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
                                
Constraint_solver::Velocity_matrix Constraint_solver::solve( const Physics_object& a, const Physics_object& b, std::vector<Contact_manifold::Contact>& contacts )
{
    Constraint_solver::Velocity_matrix result;
    result.a_linear  = a.velocity();
    result.a_angular = a.angular_velocity();
    result.b_linear  = b.velocity();
    result.b_angular = b.angular_velocity();

    for (const auto& c : contacts) {
        Math::Vector r_a = c.contact_point_a - a.coordinate_space().position();
        Math::Vector r_b = c.contact_point_b - b.coordinate_space().position();



        Constraint_solver::Velocity_matrix delta = delta_v( lagrangian_multiplier( m_time_step, 
                                    c.normal, c.contact_point_a, c.contact_point_b, r_a, r_b, 
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
