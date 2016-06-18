#include "Collision_solver.h"
#include "Physics_object.h"
#include "Physics_model.h"
#include "Minkowski_vector.h"
#include "Minkowski_simplex.h"
#include "Minkowski_polytope.h"

#include <Vector_math.h>

#include <vector>
#include <list>
#include <tuple>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

namespace {

//////////////////////////////////////////////////////////////
Math::Local_vector support( const Physics_model& model, const Math::Local_vector& direction )
{
    Math::Local_vector result;
    float max_dot = std::numeric_limits<float>::lowest();
    for (const auto& v : model.vectors()) {
        float dot = Math::dot_product( v, direction );
        if (dot > max_dot) {
            max_dot = dot;
            result = v;
        }
    }
    return result;
}


//////////////////////////////////////////////////////////////
// This function is used to check the top level models a and b.
// The children of these models are not tested at this level.
// The first step of this is to perform the GJK test to find if
// there is a collision, and then move on to EPA to find the
// collision point and normal.
// Children of models a and b are tested in other functions
bool model_intersection( const Physics_model& a, const Math::Coordinate_space& ca, 
                         const Physics_model& b, const Math::Coordinate_space& cb,
                         Minkowski_simplex& simplex )
{
    if (a.vectors().empty() || b.vectors().empty()) {
        return false;
    }
    Math::Vector direction( 1, 0, 0 );
    Math::Vector support_a  = ca.transform(support( a, ca.transform(direction) ))    + (Math::to_vector(ca.position()));
    Math::Vector support_b  = cb.transform(support( b, cb.transform(direction*-1) )) + (Math::to_vector(cb.position()));
    Math::Vector support_point = support_a - support_b;
    if (support_point == Math::Vector()) {
        // an empty simplex_point will result in an invalid direction, so try
        // another one.
        direction = Math::Vector( 0, 1, 0 );
        support_a  = ca.transform(support( a, ca.transform(direction) ))    + (Math::to_vector(ca.position()));
        support_b  = cb.transform(support( b, cb.transform(direction*-1) )) + (Math::to_vector(cb.position()));
        support_point = support_a - support_b;
    }
    simplex = Minkowski_simplex( Minkowski_vector( support_point, support_a, support_b ) );
    direction = support_point * -1;
    
    // In a perfect world this would be an infinite loop. However in reality, we can get 
    // into situations where we keep selecting the same faces over and over. If we don't
    // converge on a solution in 20 steps then just give up
    int i=0;
    for (i=0; i<20; ++i) {
        support_a  = ca.transform(support( a, ca.transform(direction) ))    + (Math::to_vector(ca.position()));
        support_b  = cb.transform(support( b, cb.transform(direction*-1) )) + (Math::to_vector(cb.position()));
        support_point = support_a - support_b;
        // If this next check is < 0 then touching will be considered
        // a collision. If it's <= 0 then touching will not be a collision.
        // For EPA to work, our GJK must exit with a tetrahedron. Therefore
        // if a contact point is directly on a line or triangle simplex (ie
        // a touching collision) that must be considered a collision in order
        // to keep building up to the tetrahedron. So this needs to be < 0
        if (Math::dot_product( support_point, direction ) < 0) {
            return false;
        }
        simplex.push_back( Minkowski_vector( support_point, support_a, support_b ) );
        bool collision_found;
        std::tie(collision_found, direction) = simplex.build();
        if (collision_found) {
            return true;
        }
    }
    // for debugging
    if (i>=20) {
        int debug = 5;
    }
    return false;
}


//////////////////////////////////////////////////////////////
// Taken from http://hacktank.net/blog/?p=119 , which was apparently
// taken from Crister Erickson's Real-Time Collision Detection
std::tuple<float,float,float> barycentric(const Math::Vector &p, const Math::Vector &a, const Math::Vector &b, const Math::Vector &c ) 
{
     Math::Vector v0 = b - a;
     Math::Vector v1 = c - a;
     Math::Vector v2 = p - a;
     float d00 = Math::dot_product( v0, v0 );
     float d01 = Math::dot_product( v0, v1 );
     float d11 = Math::dot_product( v1, v1 );
     float d20 = Math::dot_product( v2, v0 );
     float d21 = Math::dot_product( v2, v1 );
     float denom = d00 * d11 - d01 * d01;
     float v = (d11 * d20 - d01 * d21) / denom;
     float w = (d00 * d21 - d01 * d20) / denom;
     float u = 1.0f - v - w;
     return std::make_tuple( u, v, w );
}

//////////////////////////////////////////////////////////////
// Perform EPA to find the point of collision
void find_collision_point( const Physics_model& a, const Math::Coordinate_space& ca, 
                           const Physics_model& b, const Math::Coordinate_space& cb,
                           const Minkowski_simplex& simplex, Collision_solver::Contact& contact )
{
    Minkowski_polytope polytope( simplex );
    while (true) {
        float min_distance;
        Minkowski_polytope::Triangle triangle;
        std::tie(triangle,min_distance) = polytope.find_closest_triangle();
        Math::Vector direction( triangle.normal );
        Math::Vector support_a  = ca.transform(support( a, ca.transform(direction) ))    + (Math::to_vector(ca.position()));
        Math::Vector support_b  = cb.transform(support( b, cb.transform(direction*-1) )) + (Math::to_vector(cb.position()));
        Math::Vector support_point = support_a - support_b;
        if (Math::dot_product(support_point,Math::Vector(triangle.normal)) <= min_distance+0.0001f) {
            contact.normal = triangle.normal;
            contact.penetration_depth = min_distance;
            Math::Vector contact_point = Math::Vector( triangle.normal ) * min_distance;
            float u, v, w;
            std::tie(u,v,w) = barycentric( contact_point, triangle.a.v(), triangle.b.v(), triangle.c.v() );
            contact_point = triangle.a.support_a() * u +
                            triangle.b.support_a() * v +
                            triangle.c.support_a() * w;
            contact.contact_point_a = Math::to_point( contact_point );
            contact_point = triangle.a.support_b() * u +
                            triangle.b.support_b() * v +
                            triangle.c.support_b() * w;
            contact.contact_point_b = Math::to_point( contact_point );
            return;
        }
        polytope.push_back( Minkowski_vector( support_point, support_a, support_b ) );
    }
}

//////////////////////////////////////////////////////////////
bool intersection_recurse_b( const Physics_model& a, const Math::Coordinate_space& ca, 
                             const Physics_model& b, const Math::Coordinate_space& cb, 
                             std::vector<Collision_solver::Contact>& contact_manifold )
{
    bool ret_val = false;
    Minkowski_simplex simplex( std::move(Minkowski_vector()) );
    if (model_intersection( a, ca, b, cb, simplex )) {
        Collision_solver::Contact contact;
        find_collision_point( a, ca, b, cb, simplex, contact );
        contact_manifold.push_back( contact );
        ret_val = true;
    }
    for (const auto& kid : b.kids()) {
        if (intersection_recurse_b( a, ca, *kid, cb, contact_manifold )) {
            ret_val = true;
        }
    }

    return ret_val;
}

//////////////////////////////////////////////////////////////
bool intersection_recurse_a( const Physics_model& a, const Math::Coordinate_space& ca, 
                             const Physics_model& b, const Math::Coordinate_space& cb,
                             std::vector<Collision_solver::Contact>& contact_manifold )
{
    bool ret_val = false;
    if (intersection_recurse_b( a, ca, b, cb, contact_manifold )) {
        ret_val = true;
    }
    for (const auto& kid : a.kids()) {
        if (intersection_recurse_a( *kid, ca, b, cb, contact_manifold )) {
            ret_val = true;
        }
    }

    return ret_val;
}

}

//////////////////////////////////////////////////////////////
bool Collision_solver::intersection( const Physics_object& a, const Physics_object& b, std::vector<Contact>& contact_manifold )
{
    return intersection_recurse_a( *a.model(), a.coordinate_space(), *b.model(), b.coordinate_space(), contact_manifold );
}

}}

