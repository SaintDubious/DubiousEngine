#include "Constraint_solver.h"
#include "Physics_object.h"

#include <Vector_math.h>

#include <algorithm>

#include <iostream>

namespace Dubious {
namespace Physics {

Constraint_solver::Constraint_solver(float time_step, float beta, float cor, float slop)
    : m_time_step(time_step), m_beta(beta), m_coefficient_of_restitution(cor), m_slop(slop)
{
}

namespace {

// The baumgarte term will be larger for larger penetration depths. The idea here is that
// the further the colliders overlap, the more they push apart. This can be tweaked by
// changing the value of beta
float
baumgarte_term(float time_step, float beta, float penetration_depth)
{
    return -(beta / time_step) * penetration_depth;
}

// The restitution part responds stronger to objects that are moving faster. This
// will add bounce to high speed collisions, and have almost no effect for low
// speed collisions. This can be tweaked with the coefficient of restitution
float
restitution_term(const Math::Vector& n, float coefficient_of_restitution, const Math::Vector& r_a,
                 const Math::Vector& r_b, const Math::Vector& v_a, const Math::Vector& av_a,
                 const Math::Vector& v_b, const Math::Vector& av_b)
{
    Math::Vector a_part = -v_a - Math::cross_product(av_a, r_a);
    Math::Vector b_part = v_b + Math::cross_product(av_b, r_b);

    return Math::dot_product((a_part + b_part) * coefficient_of_restitution, n);
}

// The lagrangian multiplier (denoted by lambda) is a scalar derived from a heck of a
// lot of really big matrices. I'm trying my hardest to not have to create a matrix
// class for this thing, so I went ahead and unwrapped it all into a bunch of vector
// dot products.
float
lagrangian_multiplier(float dt, float beta, float cor, float slop, const Math::Vector& n,
                      float penetration_depth, const Math::Vector& r_a, const Math::Vector& r_b,
                      const Physics_object& a, const Physics_object& b)
{
    const auto& ra_x_n = Math::cross_product(r_a, n);
    const auto& rb_x_n = Math::cross_product(r_b, n);
    float       j_dot_v =
        Math::dot_product(-n, a.velocity()) + Math::dot_product(-ra_x_n, a.angular_velocity()) +
        Math::dot_product(n, b.velocity()) + Math::dot_product(rb_x_n, b.angular_velocity());

    float denom = Math::dot_product(-n * a.inverse_mass(), -n) +
                  Math::dot_product(-ra_x_n * a.inverse_moment_of_inertia(), -ra_x_n) +
                  Math::dot_product(n * b.inverse_mass(), n) +
                  Math::dot_product(rb_x_n * b.inverse_moment_of_inertia(), rb_x_n);

    float baumgarte   = 0;
    float restitution = 0;
    if (penetration_depth > slop) {
        baumgarte   = baumgarte_term(dt, beta, penetration_depth);
        restitution = restitution_term(n, cor, r_a, r_b, a.velocity(), a.angular_velocity(),
                                       b.velocity(), b.angular_velocity());
    }
    float lambda = -(j_dot_v + baumgarte + restitution) / denom;

    return lambda;
}
}  // namespace

void
Constraint_solver::warm_start(Contact_manifold& contact_manifold, float scale)
{
    Physics_object& a = contact_manifold.object_a();
    Physics_object& b = contact_manifold.object_b();
    for (const auto& c : contact_manifold.contacts()) {
        Math::Vector r_a = c.contact_point_a - a.coordinate_space().position();
        Math::Vector r_b = c.contact_point_b - b.coordinate_space().position();

        a.velocity() += c.normal_impulse * scale * a.inverse_mass() * -c.normal;
        a.angular_velocity() += c.normal_impulse * scale * a.inverse_moment_of_inertia() *
                                (Math::cross_product(-r_a, Math::Vector(c.normal)));
        b.velocity() += c.normal_impulse * scale * b.inverse_mass() * c.normal;
        b.angular_velocity() += c.normal_impulse * scale * b.inverse_moment_of_inertia() *
                                (Math::cross_product(r_b, Math::Vector(c.normal)));
    }
}

void
Constraint_solver::solve(Contact_manifold& contact_manifold)
{
    if (contact_manifold.contacts().empty()) {
        return;
    }

    Physics_object& a = contact_manifold.object_a();
    Physics_object& b = contact_manifold.object_b();

    contact_manifold.a_delta_velocity()         = Math::Vector();
    contact_manifold.a_delta_angular_velocity() = Math::Vector();
    contact_manifold.b_delta_velocity()         = Math::Vector();
    contact_manifold.b_delta_angular_velocity() = Math::Vector();

    for (auto& c : contact_manifold.contacts()) {
        Math::Vector r_a = c.contact_point_a - a.coordinate_space().position();
        Math::Vector r_b = c.contact_point_b - b.coordinate_space().position();

        float lambda = lagrangian_multiplier(m_time_step, m_beta, m_coefficient_of_restitution,
                                             m_slop, c.normal, c.penetration_depth, r_a, r_b, a, b);
        // normal impulse clamping
        float new_impulse = std::max(0.0f, c.normal_impulse + lambda);
        lambda            = new_impulse - c.normal_impulse;
        c.normal_impulse  = new_impulse;

        Math::Vector P = lambda * c.normal;

        contact_manifold.a_delta_velocity() -= P * a.inverse_mass();
        contact_manifold.a_delta_angular_velocity() -=
            a.inverse_moment_of_inertia() * Math::cross_product(r_a, P);

        contact_manifold.b_delta_velocity() += P * b.inverse_mass();
        contact_manifold.b_delta_angular_velocity() +=
            b.inverse_moment_of_inertia() * Math::cross_product(r_b, P);
    }

    const float FRICTION     = 0.01f;
    const float max_friction = FRICTION / contact_manifold.contacts().size();
    for (auto& c : contact_manifold.contacts()) {
        Math::Vector velocity = a.velocity() + contact_manifold.a_delta_velocity();
        float        speed1   = Math::dot_product(velocity, Math::Vector(c.tangent1));
        float        speed2   = Math::dot_product(velocity, Math::Vector(c.tangent2));
        contact_manifold.a_delta_velocity() -=
            (speed1 * max_friction * c.tangent1) + (speed2 * max_friction * c.tangent2);

        velocity = a.angular_velocity() + contact_manifold.a_delta_angular_velocity();
        speed1   = Math::dot_product(velocity, Math::Vector(c.normal));
        contact_manifold.a_delta_angular_velocity() -=
            (speed1 * max_friction * Math::Vector(c.normal));

        velocity = b.velocity() + contact_manifold.b_delta_velocity();
        speed1   = Math::dot_product(velocity, Math::Vector(c.tangent1));
        speed2   = Math::dot_product(velocity, Math::Vector(c.tangent2));
        contact_manifold.b_delta_velocity() -=
            (speed1 * max_friction * c.tangent1) + (speed2 * max_friction * c.tangent2);

        velocity = b.angular_velocity() + contact_manifold.b_delta_angular_velocity();
        speed1   = Math::dot_product(velocity, Math::Vector(c.normal));
        contact_manifold.b_delta_angular_velocity() -=
            (speed1 * max_friction * Math::Vector(c.normal));
    }
}

}  // namespace Physics
}  // namespace Dubious
