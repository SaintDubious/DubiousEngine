#include "Minkowski_simplex.h"

#include <Vector.h>
#include <Vector_math.h>

#include <tuple>

namespace Dubious {
namespace Physics {

Minkowski_simplex::Minkowski_simplex( const Minkowski_vector& start )
{
    m_v[0] = start;
    m_size = 1;
}

Minkowski_simplex::Minkowski_simplex()
    : m_size( 0 )
{
}

void Minkowski_simplex::push_back( const Minkowski_vector& v )
{
    m_v[m_size++] = v;
}

std::tuple<bool,Math::Vector> Minkowski_simplex::build()
{
    switch (m_size) {
    case 2:
        return build_2(); 
    case 3:
        return build_3(); 
    case 4:
        return build_4(); 
    default:
        throw std::runtime_error( "Received simplex of incorrect size" );
    }
}

// Due to the tests that created this line segment, the origin
// must be somewhere between A and B, so this is pretty trivial
std::tuple<bool,Math::Vector> Minkowski_simplex::build_2()
{
    const Math::Vector& a = m_v[1].v();
    const Math::Vector& b = m_v[0].v();
    const Math::Vector origin;

    const Math::Vector& ab = b - a;
    const Math::Vector& ao = origin - a;
    Math::Vector direction = Math::cross_product( Math::cross_product( ab, ao ), ab );
    if (direction == Math::Vector()) {
        // The point is actually on the line. In this case the direction 
        // will be an empty vector, which doesn't make any sense. Select any
        // vector perpendicular to the line
        if (Math::equals( fabs(ao.y()), 0 ) && Math::equals( fabs(ao.x()), 0 )) {
            direction = Math::Vector( 0, 1, 0 );
        }
        else {
            direction = Math::Vector( ao.y(), -ao.x(), 0 );
        } 
    }
    return std::make_tuple( false, direction );
}

std::tuple<bool,Math::Vector> Minkowski_simplex::build_3()
{
    // Winding order is important for our triangle. I've tried my 
    // best to look at this solution with both a clockwise and
    // counter-clockwise triangle and verify that the triangle
    // that results as the base of the tetrahedron will be correctly
    // wound counter-clockwise with respect to the incoming point.
    // http://hacktank.net/blog/?p=93
    const Math::Vector& a = m_v[2].v();
    const Math::Vector& b = m_v[1].v();
    const Math::Vector& c = m_v[0].v();
    const Math::Vector origin;

    const Math::Vector& ab = b - a;
    const Math::Vector& ac = c - a;
    const Math::Vector& ao = origin - a;
    const Math::Vector& ab_x_ac = Math::cross_product( ab, ac );

    const Math::Vector& ab_perp = Math::cross_product( ab, ab_x_ac );
    if (Math::dot_product( ao, ab_perp ) > 0) {
        // The point lies outside of the triangle on the ab side
        m_v[0] = m_v[1];
        m_v[1] = m_v[2];
        --m_size;
        return std::make_tuple(false,ab_perp);
    }
    const Math::Vector& ac_perp = Math::cross_product( ab_x_ac, ac );
    if (Math::dot_product( ao, ac_perp ) > 0) {
        // The point lies outside of the triangle on the ac side
        m_v[1] = m_v[2];
        --m_size;
        return std::make_tuple(false,ac_perp);
    }

    // The point lies inside the triangle
    // Now the question is if it's above or below
    float side_check = Math::dot_product( ab_x_ac, ao );
    if ( side_check > 0) {
        // This direction is above the triangle (ie on the same side
        // as the triangle normal). We keep everything the same and
        // use the direction as the triangle normal
        return std::make_tuple(false,ab_x_ac);
    }

    // This is below the triangle. We want to make sure that in
    // the tetrahedron case the new point is on the "top" of the
    // triangle, so we will reverse the winding order. 
    std::swap( m_v[0], m_v[1] );
    return std::make_tuple(false,ab_x_ac*-1);
}

std::tuple<bool,Math::Vector> Minkowski_simplex::build_4()
{
    // The winding order of the tetrahedron is important. Point A is on the
    // "top" of the triangle defined by points B, C, and D. From A's point
    // of view, the triangle is wound counter-clockwise
    const Math::Vector& a = m_v[3].v();
    const Math::Vector& b = m_v[2].v();
    const Math::Vector& c = m_v[1].v();
    const Math::Vector& d = m_v[0].v();
    const Math::Vector origin;

    const Math::Vector& ab = b-a;
    const Math::Vector& ac = c-a;
    const Math::Vector& ad = d-a;
    const Math::Vector& ao = origin-a;

    const Math::Vector& ab_x_ac = Math::cross_product( ab, ac );
    if (Math::dot_product( ab_x_ac, ao ) > 0) {
        m_v[0] = m_v[1];
        m_v[1] = m_v[2];
        m_v[2] = m_v[3];
        --m_size;
        return std::make_tuple(false,ab_x_ac);
    }
    const Math::Vector& ac_x_ad = Math::cross_product( ac, ad );
    if (Math::dot_product( ac_x_ad, ao ) > 0) {
        m_v[2] = m_v[3];
        --m_size;
        return std::make_tuple(false,ac_x_ad);
    }
    const Math::Vector& ad_x_ab = Math::cross_product( ad, ab );
    if (Math::dot_product( ad_x_ab, ao ) > 0) {
        m_v[1] = m_v[0];
        m_v[0] = m_v[2];
        m_v[2] = m_v[3];
        --m_size;
        return std::make_tuple(false,ad_x_ab);
    }

    // If the ao point is "behind" all 3 faces then it must
    // be inside the tetrahedron
    return std::make_tuple(true,Math::Vector());
}

}}
