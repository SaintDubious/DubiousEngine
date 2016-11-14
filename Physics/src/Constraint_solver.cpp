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
    Math::Vector a_part = -v_a - Math::cross_product( av_a, r_a );
    Math::Vector b_part =  v_b + Math::cross_product( av_b, r_b );

    return Math::dot_product( (a_part+b_part) * coefficient_of_restitution, n );
}

// The lagrangian multiplier (denoted by lambda) is a scalar derived from a heck of a
// lot of really big matrices. I'm trying my hardest to not have to create a matrix
// class for this thing, so I went ahead and unwrapped it all into a bunch of vector
// dot products.
float lagrangian_multiplier( float dt, float beta, float cor, float slop, 
                             const Math::Vector& n, float penetration_depth,
                             const Math::Vector& r_a, const Math::Vector& r_b,
                             const Physics_object& a, const Physics_object& b )
{
    float j_dot_v = Math::dot_product(-n, a.velocity()) + Math::dot_product(Math::cross_product(-r_a,n), a.angular_velocity()) +
                    Math::dot_product( n, b.velocity()) + Math::dot_product(Math::cross_product( r_b,n), b.angular_velocity());

    Math::Vector ra_x_n = Math::cross_product( r_a, n );
    float denom         = Math::dot_product( -n*a.inverse_mass(), -n ) + Math::dot_product( -ra_x_n*a.inverse_moment_of_inertia(), -ra_x_n);
    
    Math::Vector rb_x_n = Math::cross_product( r_b, n );
    denom              += Math::dot_product(  n*b.inverse_mass(), n)   + Math::dot_product(  rb_x_n*b.inverse_moment_of_inertia(),  rb_x_n);

    float baumgarte = 0;
    float restitution = 0;
    if (penetration_depth > slop) {
        baumgarte = baumgarte_term( dt, beta, penetration_depth ); 
        restitution = restitution_term( n, cor, r_a, r_b, a.velocity(), a.angular_velocity(), b.velocity(), b.angular_velocity() );
    }
    float lambda = -(j_dot_v + baumgarte + restitution)  / denom;

    return lambda;
}  

// Similar to the lagrangian multiplier for collision normal. However for the friction
// component, there is no baumgarte, slop, or restitution
float lagrangian_multiplier_friction( const Math::Vector& t,
                                      const Math::Vector& r_a, const Math::Vector& r_b,
                                      const Physics_object& a, const Physics_object& b )
{
    float j_dot_v = Math::dot_product(-t, a.velocity()) + Math::dot_product(Math::cross_product(-r_a,t), a.angular_velocity()) +
                    Math::dot_product( t, b.velocity()) + Math::dot_product(Math::cross_product( r_b,t), b.angular_velocity());

    Math::Vector ra_x_t = Math::cross_product( r_a, t );
    float        denom  = Math::dot_product( -t*a.inverse_mass(), -t ) + Math::dot_product( -ra_x_t*a.inverse_moment_of_inertia(), -ra_x_t);

    Math::Vector rb_x_t = Math::cross_product( r_b, t );
    denom              += Math::dot_product(  t*b.inverse_mass(),  t ) + Math::dot_product(  rb_x_t*b.inverse_moment_of_inertia(),  rb_x_t);

    return -(j_dot_v)  / denom;
}  
}
      
void Constraint_solver::warm_start( Physics_object& a, Physics_object& b, Contact_manifold& contact_manifold )
{
    for (const auto& c : contact_manifold.contacts()) {
        Math::Vector r_a = c.contact_point_a - a.coordinate_space().position();
        Math::Vector r_b = c.contact_point_b - b.coordinate_space().position();

        // friction
        a.velocity()         += c.tangent1_impulse * a.inverse_mass() * -c.tangent1 + 
                                c.tangent2_impulse * a.inverse_mass() * -c.tangent2;
        a.angular_velocity() += c.tangent1_impulse * a.inverse_moment_of_inertia() * (Math::cross_product( -r_a, Math::Vector(c.tangent1))) + 
                                c.tangent2_impulse * a.inverse_moment_of_inertia() * (Math::cross_product( -r_a, Math::Vector(c.tangent2)));
        b.velocity()         += c.tangent1_impulse * b.inverse_mass() * c.tangent1 + 
                                c.tangent2_impulse * b.inverse_mass() * c.tangent2;
        b.angular_velocity() += c.tangent1_impulse * b.inverse_moment_of_inertia() * (Math::cross_product( r_b, Math::Vector(c.tangent1))) + 
                                c.tangent2_impulse * b.inverse_moment_of_inertia() * (Math::cross_product( r_b, Math::Vector(c.tangent2)));
        // normals
        a.velocity()         += c.normal_impulse * a.inverse_mass() * -c.normal;
        a.angular_velocity() += c.normal_impulse * a.inverse_moment_of_inertia() * (Math::cross_product( -r_a, Math::Vector(c.normal)));
        b.velocity()         += c.normal_impulse * b.inverse_mass() * c.normal;
        b.angular_velocity() += c.normal_impulse * b.inverse_moment_of_inertia() * (Math::cross_product( r_b, Math::Vector(c.normal)));
    }
}
                          
void Constraint_solver::solve( Physics_object& a, Physics_object& b, Contact_manifold& contact_manifold )
{
    for (auto& c : contact_manifold.contacts()) {
        Math::Vector r_a = c.contact_point_a - a.coordinate_space().position();
        Math::Vector r_b = c.contact_point_b - b.coordinate_space().position();

        const float FRICTION = 0.3f;
        float max_friction = FRICTION * c.normal_impulse;

        float lambda1      = lagrangian_multiplier_friction( c.tangent1, r_a, r_b, a, b );
        float new_impulse  = std::max( -max_friction, std::min( max_friction, c.tangent1_impulse + lambda1 ));
        lambda1            = new_impulse - c.tangent1_impulse;
        c.tangent1_impulse = new_impulse;

        float lambda2      = lagrangian_multiplier_friction( c.tangent2, r_a, r_b, a, b );
        new_impulse        = std::max( -max_friction, std::min( max_friction, c.tangent2_impulse + lambda2 ));
        lambda2            = new_impulse - c.tangent2_impulse;
        c.tangent2_impulse = new_impulse;
        
        a.velocity()         += lambda1 * a.inverse_mass() * -c.tangent1 + 
                                lambda2 * a.inverse_mass() * -c.tangent2;
        a.angular_velocity() += lambda1 * a.inverse_moment_of_inertia() * (Math::cross_product( -r_a, Math::Vector(c.tangent1))) + 
                                lambda2 * a.inverse_moment_of_inertia() * (Math::cross_product( -r_a, Math::Vector(c.tangent2)));
        b.velocity()         += lambda1 * b.inverse_mass() * c.tangent1 + 
                                lambda2 * b.inverse_mass() * c.tangent2;
        b.angular_velocity() += lambda1 * b.inverse_moment_of_inertia() * (Math::cross_product( r_b, Math::Vector(c.tangent1))) + 
                                lambda2 * b.inverse_moment_of_inertia() * (Math::cross_product( r_b, Math::Vector(c.tangent2)));
    }

    for (auto& c : contact_manifold.contacts()) {
        Math::Vector r_a = c.contact_point_a - a.coordinate_space().position();
        Math::Vector r_b = c.contact_point_b - b.coordinate_space().position();

        float lambda = lagrangian_multiplier( m_time_step, m_beta, m_coefficient_of_restitution, m_slop,
                                              c.normal, c.penetration_depth, r_a, r_b, a, b );
        // normal impulse clamping
        float new_impulse = std::max( 0.0f, c.normal_impulse + lambda );
        lambda = new_impulse - c.normal_impulse;
        c.normal_impulse = new_impulse;

        a.velocity()         += lambda * a.inverse_mass() * -c.normal;
        a.angular_velocity() += lambda * a.inverse_moment_of_inertia() * (Math::cross_product( -r_a, Math::Vector(c.normal)));
        b.velocity()         += lambda * b.inverse_mass() * c.normal;
        b.angular_velocity() += lambda * b.inverse_moment_of_inertia() * (Math::cross_product( r_b, Math::Vector(c.normal)));

    }
}

}}
