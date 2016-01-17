#include "Collision_solver.h"
#include "Physics_object.h"

#include <Vector_math.h>

#include <vector>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

//////////////////////////////////////////////////////////////
Math::Vector support( const Physics_object& object, const Math::Vector& direction )
{
    Math::Vector result;
    float max_dot = std::numeric_limits<float>::lowest();
    for (const auto& v : object.cached_collision_vectors()) {
        float dot = Math::dot_product( v, direction );
        if (dot > max_dot) {
            max_dot = dot;
            result = v;
        }
    }
    return result;
}

//////////////////////////////////////////////////////////////
bool nearest_simplex_2( std::vector<Math::Vector>& simplex, Math::Vector& direction )
{
    const Math::Vector& a = simplex[1];
    const Math::Vector& b = simplex[0];
    const Math::Vector origin;

    const Math::Vector& ab = b - a;
    const Math::Vector& ao = origin - a;
    direction = Math::cross_product( Math::cross_product( ab, ao ), ab );
    if (direction == Math::Vector()) {
        // If the cross product is (0,0,0) then the origin lies on this
        // line. No need to go any further.
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////
bool nearest_simplex_3( std::vector<Math::Vector>& simplex, Math::Vector& direction )
{
    // Winding order is important for our triangle. I've tried my 
    // best to look at this solution with both a clockwise and
    // counter-clockwise triangle and verify that the triangle
    // that results as the base of the tetrahedron will be correctly
    // wound counter-clockwise with respect to the incoming point.
    // http://hacktank.net/blog/?p=93
    const Math::Vector& a = simplex[2];
    const Math::Vector& b = simplex[1];
    const Math::Vector& c = simplex[0];
    const Math::Vector origin;

    const Math::Vector& ab = b - a;
    const Math::Vector& ac = c - a;
    const Math::Vector& ao = origin - a;
    const Math::Vector& ab_x_ac = Math::cross_product( ab, ac );

    const Math::Vector& ab_perp = Math::cross_product( ab, ab_x_ac );
    if (Math::dot_product( ao, ab_perp ) > 0) {
        // The point lies outside of the triangle on the ab side
        direction = ab_perp;
        simplex[0] = simplex[1];
        simplex[1] = simplex[2];
        simplex.pop_back();
        return false;
    }
    const Math::Vector& ac_perp = Math::cross_product( ab_x_ac, ac );
    if (Math::dot_product( ao, ac_perp ) > 0) {
        // The point lies outside of the triangle on the ac side
        direction = ac_perp;
        simplex[1] = simplex[2];
        simplex.pop_back();
        return false;
    }

    // The point lies inside the triangle
    // Now the question is if it's above or below
    float side_check = Math::dot_product( ab_x_ac, ao );
    if (Math::equals( side_check, 0 )) {
        // If the side check finds that the point lies 
        // within this triangle then there's no need
        // to continue
        return true;
    }
    else if ( side_check > 0) {
        // This direction is above the triangle (ie on the same side
        // as the triangle normal). We keep everything the same and
        // use the direction as the triangle normal
        direction = ab_x_ac;
    }
    else {
        // This is below the triangle. We want to make sure that in
        // the tetrahedron case the new point is on the "top" of the
        // triangle, so we will reverse the winding order. 
        Math::Vector hold = simplex[0];
        simplex[0] = simplex[1];
        simplex[1] = hold;
        direction = ab_x_ac*-1;
    }

    return false;
}

//////////////////////////////////////////////////////////////
bool nearest_simplex_4( std::vector<Math::Vector>& simplex, Math::Vector& direction )
{
    // The winding order of the tetrahedron is important. Point A is on the
    // "top" of the triangle defined by points B, C, and D. From A's point
    // of view, the triangle is wound counter-clockwise
    const Math::Vector& a = simplex[3];
    const Math::Vector& b = simplex[2];
    const Math::Vector& c = simplex[1];
    const Math::Vector& d = simplex[0];
    const Math::Vector origin;

    const Math::Vector& ab = b-a;
    const Math::Vector& ac = c-a;
    const Math::Vector& ad = d-a;
    const Math::Vector& ao = origin-a;

    const Math::Vector& ab_x_ac = Math::cross_product( ab, ac );
    if (Math::dot_product( ab_x_ac, ao ) > 0) {
        direction = ab_x_ac;
        simplex[0] = simplex[1];
        simplex[1] = simplex[2];
        simplex[2] = simplex[3];
        simplex.pop_back();
        return false;
    }
    const Math::Vector& ac_x_ad = Math::cross_product( ac, ad );
    if (Math::dot_product( ac_x_ad, ao ) > 0) {
        direction = ac_x_ad;
        simplex[2] = simplex[3];
        simplex.pop_back();
        return false;
    }
    const Math::Vector& ad_x_ab = Math::cross_product( ad, ab );
    if (Math::dot_product( ad_x_ab, ao ) > 0) {
        direction = ad_x_ab;
        simplex[1] = simplex[0];
        simplex[0] = simplex[2];
        simplex[2] = simplex[3];
        simplex.pop_back();
        return false;
    }

    // If the ao point is "behind" all 3 faces then it must
    // be inside the tetrahedron
    return true;
}

//////////////////////////////////////////////////////////////
bool Nearestsimplex( std::vector<Math::Vector>& simplex, Math::Vector& direction )
{
    switch (simplex.size()) {
    case 2:
        return nearest_simplex_2( simplex, direction ); 
    case 3:
        return nearest_simplex_3( simplex, direction ); 
    case 4:
        return nearest_simplex_4( simplex, direction ); 
    default:
        throw std::runtime_error( "Received simplex of incorrect size" );
    }
}

//////////////////////////////////////////////////////////////
bool Collision_solver::intersection( const Physics_object& a, const Physics_object& b )
{
    a.cache_collision_vectors();
    b.cache_collision_vectors();

    Math::Vector direction( 1, 0, 0 );
    Math::Vector simplex_point = support( a, direction ) - support( b, direction*-1 );
    std::vector<Math::Vector> simplex;
    simplex.push_back( simplex_point );
    direction = Math::Vector() - simplex_point;
    while (true) {
        simplex_point = support( a, direction ) - support( b, direction*-1 );
        // If this next check is < 0 then touching will be considered
        // a collision. If it's <= 0 then thouching will not be a collision
        // .... I think. Not very well tested
        if (Math::dot_product( simplex_point, direction ) <= 0) {
            return false;
        }
        simplex.push_back( simplex_point );
        if (Nearestsimplex( simplex, direction )) {
            return true;
        }
    }
    
    return false;
}

}}

