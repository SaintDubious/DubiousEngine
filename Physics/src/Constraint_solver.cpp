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

// We take a local copy of an object's velocity so that we can update it without writing back to the
// underlying object. This allows us to resolve collision in parallel
struct Object {
    Object(const Physics_object& p)
        : v(p.velocity())
        , w(p.angular_velocity())
        , inverse_mass(p.inverse_mass())
        , inverse_moment_of_inertia(p.inverse_moment_of_inertia())
    {
    }
    Math::Vector v;
    Math::Vector w;
    float        inverse_mass;
    float        inverse_moment_of_inertia;
};

// This seems to be a pretty vanilla formula for impulse I found at
// https://www.euclideanspace.com/physics/dynamics/collision/threed/index.htm
//
// It's been augmented by the baumgarte and restitution ideas I've picked up from Erin Cotto's work.
// These are both fake forces to respond when normal simulation breaks down
float
impulse(const Math::Vector& n, const Math::Vector& r_a, const Math::Vector& r_b, const Object& a,
        const Object& b, float penetration_depth, float slop, float dt, float beta, float cor)
{
    const Math::Vector& va         = a.v;
    const Math::Vector& wa         = a.w;
    const Math::Vector& vb         = b.v;
    const Math::Vector& wb         = b.w;
    const Math::Vector& ra_x_n     = Math::cross_product(r_a, n);
    const Math::Vector& rb_x_n     = Math::cross_product(r_b, n);
    const float         inverse_ma = a.inverse_mass;
    const float         inverse_mb = b.inverse_mass;
    const float         inverse_ia = a.inverse_moment_of_inertia;
    const float         inverse_ib = b.inverse_moment_of_inertia;

    // baumgarte and restitution are fake forces.
    //  - baumgarte   : greater then penetration depth is deeper
    //  - restitution : creater when incident collision velocity is greater
    float baumgarte   = 0;
    float restitution = 0;
    if (penetration_depth > slop) {
        baumgarte   = baumgarte_term(dt, beta, penetration_depth);
        restitution = restitution_term(n, cor, r_a, r_b, va, wa, vb, wb);
    }

    return -1 *
           (baumgarte + restitution +
            (Math::dot_product((vb - va), n) + Math::dot_product(rb_x_n, wb) -
             Math::dot_product(ra_x_n, wa))) /
           (inverse_ma + inverse_mb + Math::dot_product(ra_x_n, inverse_ia * ra_x_n) +
            Math::dot_product(rb_x_n, inverse_ib * rb_x_n));
}

float
friction_impulse(const Math::Vector& t, const Math::Vector& r_a, const Math::Vector& r_b,
                 const Object& a, const Object& b)
{
    return impulse(t, r_a, r_b, a, b, 0, 0, 0, 0, 0);
}

}  // namespace

void
Constraint_solver::warm_start(Contact_manifold& contact_manifold)
{
    Physics_object& a = contact_manifold.object_a();
    Physics_object& b = contact_manifold.object_b();
    for (const auto& c : contact_manifold.contacts()) {
        Math::Vector r_a = c.contact_point_a - a.coordinate_space().position();
        Math::Vector r_b = c.contact_point_b - b.coordinate_space().position();

        a.velocity() += c.normal_impulse * a.inverse_mass() * -c.normal;
        a.angular_velocity() += c.normal_impulse * a.inverse_moment_of_inertia() *
                                (Math::cross_product(-r_a, Math::Vector(c.normal)));
        b.velocity() += c.normal_impulse * b.inverse_mass() * c.normal;
        b.angular_velocity() += c.normal_impulse * b.inverse_moment_of_inertia() *
                                (Math::cross_product(r_b, Math::Vector(c.normal)));
    }
}

void
Constraint_solver::solve(Contact_manifold& contact_manifold)
{
    if (contact_manifold.contacts().empty()) {
        return;
    }

    const Physics_object& a = contact_manifold.object_a();
    Object                obj_a(a);
    const Physics_object& b = contact_manifold.object_b();
    Object                obj_b(b);

    for (auto& c : contact_manifold.contacts()) {
        Math::Vector r_a = c.contact_point_a - a.coordinate_space().position();
        Math::Vector r_b = c.contact_point_b - b.coordinate_space().position();

        float lambda = impulse(c.normal, r_a, r_b, obj_a, obj_b, c.penetration_depth, m_slop,
                               m_time_step, m_beta, m_coefficient_of_restitution);

        // normal impulse clamping
        float new_impulse = std::max(0.0f, c.normal_impulse + lambda);
        lambda            = new_impulse - c.normal_impulse;
        c.normal_impulse  = new_impulse;

        Math::Vector P = lambda * c.normal;

        obj_a.v -= P * a.inverse_mass();
        obj_a.w -= a.inverse_moment_of_inertia() * Math::cross_product(r_a, P);

        obj_b.v += P * b.inverse_mass();
        obj_b.w += b.inverse_moment_of_inertia() * Math::cross_product(r_b, P);
    }

    for (auto& c : contact_manifold.contacts()) {
        Math::Vector r_a = c.contact_point_a - a.coordinate_space().position();
        Math::Vector r_b = c.contact_point_b - b.coordinate_space().position();

        const float FRICTION = 0.3f;
        //  const float FRICTION     = 0.0f;
        float max_friction = FRICTION * c.normal_impulse;

        float lambda1 = friction_impulse(c.tangent1, r_a, r_b, obj_a, obj_b);
        float new_impulse =
            std::max(-max_friction, std::min(max_friction, c.tangent1_impulse + lambda1));
        lambda1            = new_impulse - c.tangent1_impulse;
        c.tangent1_impulse = new_impulse;

        float lambda2 = friction_impulse(c.tangent2, r_a, r_b, obj_a, obj_b);
        new_impulse = std::max(-max_friction, std::min(max_friction, c.tangent2_impulse + lambda2));
        lambda2     = new_impulse - c.tangent2_impulse;
        c.tangent2_impulse = new_impulse;

        Math::Vector P1 = lambda1 * c.tangent1;
        Math::Vector P2 = lambda2 * c.tangent2;

        obj_a.v -= P1 * a.inverse_mass() + P2 * a.inverse_mass();
        obj_a.w -= a.inverse_moment_of_inertia() * (Math::cross_product(r_a, P1)) +
                   a.inverse_moment_of_inertia() * (Math::cross_product(r_a, P2));

        obj_b.v += P1 * b.inverse_mass() + P2 * b.inverse_mass();
        obj_b.w += b.inverse_moment_of_inertia() * (Math::cross_product(r_b, P1)) +
                   b.inverse_moment_of_inertia() * (Math::cross_product(r_b, P2));
    }
    contact_manifold.a_delta_velocity()         = obj_a.v - a.velocity();
    contact_manifold.a_delta_angular_velocity() = obj_a.w - a.angular_velocity();
    contact_manifold.b_delta_velocity()         = obj_b.v - b.velocity();
    contact_manifold.b_delta_angular_velocity() = obj_b.w - b.angular_velocity();
}

}  // namespace Physics
}  // namespace Dubious
