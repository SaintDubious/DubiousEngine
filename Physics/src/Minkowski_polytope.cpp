#include "Minkowski_polytope.h"

#include <tuple>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

//////////////////////////////////////////////////////////////
Minkowski_polytope::Minkowski_polytope( const Minkowski_simplex& simplex )
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
std::tuple<Minkowski_polytope::Triangle,float> Minkowski_polytope::find_closest_triangle()
{
    float distance = std::numeric_limits<float>::max();
    Triangle ret = m_triangles.front();
    for (const auto& t : m_triangles) {
        float dot = Math::dot_product( t.a.v(), Math::Vector(t.normal) );
        if ( dot < distance) {
            ret = t;
            distance = dot;
        }
    }
    return std::make_tuple(ret, distance);
}

//////////////////////////////////////////////////////////////
void Minkowski_polytope::push_back( Minkowski_vector&& v )
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
void Minkowski_polytope::push_edge( const Edge& edge )
{
    for (auto iter=m_edges.cbegin(), end=m_edges.cend(); iter!=end; ++iter) {
        if (iter->a.v() == edge.b.v() && iter->b.v() == edge.a.v()) {
            m_edges.erase( iter );
            return;
        }
    }
    m_edges.push_back( edge );
}

}}

