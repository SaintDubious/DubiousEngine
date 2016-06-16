#include "Collision_solver.h"
#include "Physics_object.h"
#include "Physics_model.h"

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
// In the EPA algorithm we need to keep track of the "Minkowski Point"
// ie the point on the Minkowski Polytope, as well as the support points 
// in global space that corresponds to this point. 
// The bulk of GJK and EPA will only use the v() accessor, but once
// the contact point is found (on the Minkowski polytope), the support  
// points will be needed to find that point in global space
class Minkowski_vector {
public:
    Minkowski_vector( const Math::Vector& v, const Math::Vector& support_a, const Math::Vector& support_b )
        : m_v( v )
        , m_support_a( support_a )
        , m_support_b( support_b )
    {}

    const Math::Vector& v() const { return m_v; }
    const Math::Vector& support_a() const { return m_support_a; }
    const Math::Vector& support_b() const { return m_support_b; }

private:
    Math::Vector m_v;
    Math::Vector m_support_a;
    Math::Vector m_support_b;
};

//////////////////////////////////////////////////////////////
// The Simplex is the backbone of the GJK algorithm. It is comprised
// of 2 to 4 points, representing a line, triangle, or tetrahedron.
// I made it a separate class mostly so that it can be used in the 
// constructor of the Polytope created in the EPA step which follows
// after a collision is found. In order to be used to construct the
// polytope, it must exist GJK as a tetrahedron. 
class Simplex {
public:
    //////////////////////////////////////////////////////////////
    Simplex( Minkowski_vector&& start )
        : m_v { start }
    {
    }

    //////////////////////////////////////////////////////////////
    void push_back( Minkowski_vector&& v )
    {
        m_v.push_back( v );
    }

    //////////////////////////////////////////////////////////////
    bool nearest( Math::Vector& direction )
    {
        switch (m_v.size()) {
        case 2:
            return nearest_2( direction ); 
        case 3:
            return nearest_3( direction ); 
        case 4:
            return nearest_4( direction ); 
        default:
            throw std::runtime_error( "Received simplex of incorrect size" );
        }
    }

    //////////////////////////////////////////////////////////////
    const std::vector<Minkowski_vector>& v() const { return m_v; }

private:

    //////////////////////////////////////////////////////////////
    // Due to the tests that created this line segment, the origin
    // must be somewhere between A and B, so this is pretty trivial
    bool nearest_2( Math::Vector& direction )
    {
        const Math::Vector& a = m_v[1].v();
        const Math::Vector& b = m_v[0].v();
        const Math::Vector origin;

        const Math::Vector& ab = b - a;
        const Math::Vector& ao = origin - a;
        direction = Math::cross_product( Math::cross_product( ab, ao ), ab );
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
        return false;
    }

    //////////////////////////////////////////////////////////////
    bool nearest_3( Math::Vector& direction )
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
            direction = ab_perp;
            m_v[0] = m_v[1];
            m_v[1] = m_v[2];
            m_v.pop_back();
            return false;
        }
        const Math::Vector& ac_perp = Math::cross_product( ab_x_ac, ac );
        if (Math::dot_product( ao, ac_perp ) > 0) {
            // The point lies outside of the triangle on the ac side
            direction = ac_perp;
            m_v[1] = m_v[2];
            m_v.pop_back();
            return false;
        }

        // The point lies inside the triangle
        // Now the question is if it's above or below
        float side_check = Math::dot_product( ab_x_ac, ao );
        if ( side_check > 0) {
            // This direction is above the triangle (ie on the same side
            // as the triangle normal). We keep everything the same and
            // use the direction as the triangle normal
            direction = ab_x_ac;
        }
        else {
            // This is below the triangle. We want to make sure that in
            // the tetrahedron case the new point is on the "top" of the
            // triangle, so we will reverse the winding order. 
            std::swap( m_v[0], m_v[1] );
            direction = ab_x_ac*-1;
        }

        return false;
    }

    //////////////////////////////////////////////////////////////
    bool nearest_4( Math::Vector& direction )
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
            direction = ab_x_ac;
            m_v[0] = m_v[1];
            m_v[1] = m_v[2];
            m_v[2] = m_v[3];
            m_v.pop_back();
            return false;
        }
        const Math::Vector& ac_x_ad = Math::cross_product( ac, ad );
        if (Math::dot_product( ac_x_ad, ao ) > 0) {
            direction = ac_x_ad;
            m_v[2] = m_v[3];
            m_v.pop_back();
            return false;
        }
        const Math::Vector& ad_x_ab = Math::cross_product( ad, ab );
        if (Math::dot_product( ad_x_ab, ao ) > 0) {
            direction = ad_x_ab;
            m_v[1] = m_v[0];
            m_v[0] = m_v[2];
            m_v[2] = m_v[3];
            m_v.pop_back();
            return false;
        }

        // If the ao point is "behind" all 3 faces then it must
        // be inside the tetrahedron
        return true;
    }

    std::vector<Minkowski_vector> m_v;
};

//////////////////////////////////////////////////////////////
// This function is used to check the top level models a and b.
// The children of these models are not tested at this level.
// The first step of this is to perform the GJK test to find if
// there is a collision, and then move on to EPA to find the
// collision point and normal.
// Children of models a and b are tested in other functions
bool model_intersection( const Physics_model& a, const Math::Coordinate_space& ca, 
                         const Physics_model& b, const Math::Coordinate_space& cb,
                         Simplex& simplex )
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
    simplex = Simplex( Minkowski_vector( support_point, support_a, support_b ) );
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
        if (simplex.nearest( direction )) {
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
class Polytope {
public:
    Polytope( const Simplex& simplex )
    {
        if (simplex.v().size() != 4) {
            throw std::runtime_error( "Attempting to create polytope from simplex that is not tetrahedron" );
        }
        const Minkowski_vector& a = simplex.v()[3];
        const Minkowski_vector& b = simplex.v()[2];
        const Minkowski_vector& c = simplex.v()[1];
        const Minkowski_vector& d = simplex.v()[0];
        m_triangles.push_back( Triangle( a, b, c ) );
        m_triangles.push_back( Triangle( a, c, d ) );
        m_triangles.push_back( Triangle( a, d, b ) );
        m_triangles.push_back( Triangle( b, d, c ) );
    }

    //////////////////////////////////////////////////////////////
    struct Triangle {
        Triangle( const Minkowski_vector& p1, const Minkowski_vector& p2, const Minkowski_vector& p3 )
            : a( p1 )
            , b( p2 )
            , c( p3 ) 
        {
            normal = Math::cross_product( b.v()-a.v(), c.v()-a.v() );
        }

        Minkowski_vector  a;
        Minkowski_vector  b;
        Minkowski_vector  c;
        Math::Unit_vector normal;
    };

    //////////////////////////////////////////////////////////////
    Triangle find_closest_triangle( float& distance )
    {
        distance = std::numeric_limits<float>::max();
        Triangle ret = m_triangles.front();
        for (const auto& t : m_triangles) {
            float dot = Math::dot_product( t.a.v(), Math::Vector(t.normal) );
            if ( dot < distance) {
                ret = t;
                distance = dot;
            }
        }
        return ret;
    }

    //////////////////////////////////////////////////////////////
    void push_back( Minkowski_vector&& v )
    {
        m_edges.clear();
        for (auto iter=m_triangles.cbegin(), end=m_triangles.cend(); iter!=end; ) {
            if (Math::dot_product( Math::Vector(iter->normal), v.v()-iter->a.v() ) > 0) {
                // This triangle can be "seen" from the new point, it needs to
                // be removed.
                push_edge( Edge( iter->a, iter->b ) );
                push_edge( Edge( iter->b, iter->c ) );
                push_edge( Edge( iter->c, iter->a ) );
                iter = m_triangles.erase( iter );
            }
            else {
                ++iter;
            }
        } 
        for (const auto& e : m_edges) {
            m_triangles.push_back( Triangle( v, e.a, e.b ) );
        }
    }

private:


    //////////////////////////////////////////////////////////////
    struct Edge {
        Edge( const Minkowski_vector& p1, const Minkowski_vector& p2 )
            : a( p1 )
            , b( p2 )
        {}
        Minkowski_vector a;
        Minkowski_vector b;
    };

    //////////////////////////////////////////////////////////////
    void push_edge( const Edge& edge )
    {
        for (auto iter=m_edges.cbegin(), end=m_edges.cend(); iter!=end; ++iter) {
            if (iter->a.v() == edge.b.v() && iter->b.v() == edge.a.v()) {
                m_edges.erase( iter );
                return;
            }
        }
        m_edges.push_back( edge );
    }

    std::list<Triangle> m_triangles;
    std::list<Edge> m_edges;
};

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
                           const Simplex& simplex, Collision_solver::Contact& contact )
{
    Polytope polytope( simplex );
    while (true) {
        float min_distance;
        const Polytope::Triangle& triangle = polytope.find_closest_triangle( min_distance );
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
    Math::Vector tmp;
    Simplex simplex( Minkowski_vector(tmp,tmp,tmp) );
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

