#include "Constraint_solver.h"
#include "Physics_object.h"

#include <Vector_math.h>

#include <algorithm>

#include <iostream>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

Constraint_solver::Constraint_solver( float time_step, float beta, float cor, float slop )
    : m_time_step( time_step )
    , m_beta( beta )
    , m_coefficient_of_restitution( cor )
    , m_slop( slop )
{
}

namespace {

// The baumgarte term will be larger for larger penetration depths. The idea here is that
// the further the colliders overlap, the more they push apart. This can be tweaked by
// changing the value of beta
float baumgarte_term( float time_step, float beta, float penetration_depth )
{
    return -(beta/time_step) * penetration_depth;
}

// The restitution part responds stronger to objects that are moving faster. This
// will add bounce to high speed collisions, and have almost no effect for low
// speed collisions. This can be tweaked with the coefficient of restitution
float restitution_term( const Math::Vector& n, float coefficient_of_restitution,
                        const Math::Vector& r_a, const Math::Vector& r_b,
                        const Math::Vector& v_a, const Math::Vector& av_a, 
                        const Math::Vector& v_b, const Math::Vector& av_b)
{
    Math::Vector a_part = -1*v_a - Math::cross_product( av_a, r_a );
    Math::Vector b_part =    v_b + Math::cross_product( av_b, r_b );

    return Math::dot_product( (a_part+b_part) * coefficient_of_restitution, n );
}

// The lagrangian multiplier (denoted by lambda) is a scalar derived from a heck of a
// lot of really big matrices. I'm trying my hardest to not have to create a matrix
// class for this thing, so I went ahead and unwrapped it all into a bunch of vector
// dot products.
float lagrangian_multiplier( float dt, float beta, float cor, float slop, 
                             const Math::Vector& n, float penetration_depth,
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

    float baumgarte = 0;
    if (penetration_depth > slop) {
        baumgarte = baumgarte_term( dt, beta, penetration_depth ); 
    }
    float restitution = restitution_term( n, cor, r_a, r_b, v_a, av_a, v_b, av_b );
    float lambda = -(j_dot_v + baumgarte + restitution)  / denom;

    return lambda;
}  

// The lagrangian multiplier (denoted by lambda) is a scalar derived from a heck of a
// lot of really big matrices. I'm trying my hardest to not have to create a matrix
// class for this thing, so I went ahead and unwrapped it all into a bunch of vector
// dot products.
float lagrangian_multiplier_friction( 
                             const Math::Vector& t,
                             const Math::Vector& r_a, const Math::Vector& r_b,
                             const Math::Vector& v_a, const Math::Vector& av_a, 
                             float mass_a, float inertial_tensor_a,
                             const Math::Vector& v_b, const Math::Vector& av_b,
                             float mass_b, float inertial_tensor_b )
{
    float j_dot_v = Math::dot_product((-1*t), v_a) + Math::dot_product(Math::cross_product(-1*r_a,t), av_a) +
                    Math::dot_product(t, v_b)      + Math::dot_product(Math::cross_product(r_b,t),av_b);
    float inverse_m_a = 1.0f/mass_a;
    float inverse_m_b = 1.0f/mass_b;
    float inverse_tensor_a = 1.0f/inertial_tensor_a;
    float inverse_tensor_b = 1.0f/inertial_tensor_b;

    float denom_a = 0;
    if (!Physics_object::is_stationary(mass_a)) {
        Math::Vector negra_x_t = Math::cross_product( -1*r_a, t );
        denom_a = Math::dot_product( -1*t*inverse_m_a, -1*t ) + Math::dot_product( negra_x_t*inverse_tensor_a, negra_x_t);
    }
    float denom_b = 0;
    if (!Physics_object::is_stationary(mass_b)) {
        Math::Vector rb_x_t = Math::cross_product( r_b, t );
        denom_b = Math::dot_product(t*inverse_m_b, t) + Math::dot_product( rb_x_t*inverse_tensor_b, rb_x_t);
    }
    float denom = denom_a + denom_b;

    float lambda = -(j_dot_v)  / denom;

    return lambda;
}  

struct Velocity_matrix {
    Math::Vector    a_linear;
    Math::Vector    a_angular;
    Math::Vector    b_linear;
    Math::Vector    b_angular;
};

Velocity_matrix delta_v( float lambda, const Math::Vector& n, 
                                            const Math::Vector& r_a, const Math::Vector& r_b,
                                            float mass_a, float inertial_tensor_a,
                                            float mass_b, float inertial_tensor_b )
{
    Velocity_matrix result;
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
      
void Constraint_solver::warm_start( Physics_object& a, Physics_object& b, Contact_manifold& contact_manifold )
{
    for (const auto& c : contact_manifold.contacts()) {
        Math::Vector r_a = c.contact_point_a - a.coordinate_space().position();
        Math::Vector r_b = c.contact_point_b - b.coordinate_space().position();

        Velocity_matrix delta  = delta_v( c.tangent_impulse, Math::Unit_vector(c.tangent1+c.tangent2), r_a, r_b, a.mass(), a.moment_of_inertia(), 
                                          b.mass(), b.moment_of_inertia() );
        a.velocity()           = a.velocity()          + delta.a_linear;
        a.angular_velocity()   = a.angular_velocity()  + delta.a_angular;
        b.velocity()           = b.velocity()          + delta.b_linear;
        b.angular_velocity()   = b.angular_velocity()  + delta.b_angular;

        delta  = delta_v( c.normal_impulse, c.normal, r_a, r_b, a.mass(), a.moment_of_inertia(), 
                                          b.mass(), b.moment_of_inertia() );
        a.velocity()           = a.velocity()          + delta.a_linear;
        a.angular_velocity()   = a.angular_velocity()  + delta.a_angular;
        b.velocity()           = b.velocity()          + delta.b_linear;
        b.angular_velocity()   = b.angular_velocity()  + delta.b_angular;
    }
}
                          
void Constraint_solver::solve( Physics_object& a, Physics_object& b, Contact_manifold& contact_manifold )
{
    for (auto& c : contact_manifold.contacts()) {
        Math::Vector r_a = c.contact_point_a - a.coordinate_space().position();
        Math::Vector r_b = c.contact_point_b - b.coordinate_space().position();

        float lambda = lagrangian_multiplier_friction( Math::Unit_vector(c.tangent1+c.tangent2), 
                                              r_a, r_b, a.velocity(), a.angular_velocity(), a.mass(), a.moment_of_inertia(),
                                              b.velocity(), b.angular_velocity(), b.mass(), b.moment_of_inertia());
        // normal impulse clamping
        const float FRICTION = 0.3f;
        float max_friction = FRICTION * c.normal_impulse;


        float new_impulse = c.tangent_impulse + lambda;
        if (new_impulse < -max_friction) {
            new_impulse = -max_friction;
        }
        else if (new_impulse > max_friction) {
            new_impulse = max_friction;
        }



        lambda = new_impulse - c.tangent_impulse;
        c.tangent_impulse = new_impulse;

        Velocity_matrix delta  = delta_v( lambda, c.tangent1+c.tangent2, r_a, r_b, a.mass(), a.moment_of_inertia(), 
                                          b.mass(), b.moment_of_inertia() );
         
        a.velocity()           = a.velocity()          + delta.a_linear;
        a.angular_velocity()   = a.angular_velocity()  + delta.a_angular;
        b.velocity()           = b.velocity()          + delta.b_linear;
        b.angular_velocity()   = b.angular_velocity()  + delta.b_angular;
    }

    for (auto& c : contact_manifold.contacts()) {
        Math::Vector r_a = c.contact_point_a - a.coordinate_space().position();
        Math::Vector r_b = c.contact_point_b - b.coordinate_space().position();

        float lambda = lagrangian_multiplier( m_time_step, m_beta, m_coefficient_of_restitution, m_slop,
                                              c.normal, c.penetration_depth, c.contact_point_a, c.contact_point_b, 
                                              r_a, r_b, a.velocity(), a.angular_velocity(), a.mass(), a.moment_of_inertia(),
                                              b.velocity(), b.angular_velocity(), b.mass(), b.moment_of_inertia());
        // normal impulse clamping
        float new_impulse = std::max( 0.0f, c.normal_impulse + lambda );
        lambda = new_impulse - c.normal_impulse;
        c.normal_impulse = new_impulse;

        Velocity_matrix delta  = delta_v( lambda, c.normal, r_a, r_b, a.mass(), a.moment_of_inertia(), 
                                          b.mass(), b.moment_of_inertia() );
         
        a.velocity()           = a.velocity()          + delta.a_linear;
        a.angular_velocity()   = a.angular_velocity()  + delta.a_angular;
        b.velocity()           = b.velocity()          + delta.b_linear;
        b.angular_velocity()   = b.angular_velocity()  + delta.b_angular;
    }
}

}}
