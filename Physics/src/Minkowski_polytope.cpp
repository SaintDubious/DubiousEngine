#include "Minkowski_polytope.h"

#include <tuple>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

//////////////////////////////////////////////////////////////
Minkowski_polytope::Minkowski_polytope( const Minkowski_simplex& simplex )
{
    if (simplex.size() != 4) {
        throw std::runtime_error( "Attempting to create polytope from simplex that is not tetrahedron" );
    }
    const Minkowski_vector& a = simplex.v()[3];
    const Minkowski_vector& b = simplex.v()[2];
    const Minkowski_vector& c = simplex.v()[1];
    const Minkowski_vector& d = simplex.v()[0];
    m_triangles.reserve( 10 );
    m_triangles.push_back( Triangle( a, b, c ) );
    m_triangles.push_back( Triangle( a, c, d ) );
    m_triangles.push_back( Triangle( a, d, b ) );
    m_triangles.push_back( Triangle( b, d, c ) );
}

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
    m_edges.reserve( 10 );
    for (auto iter=m_triangles.cbegin(); iter!=m_triangles.cend(); ) {
        // I've seen cases of touching objects where this dot product comes out
        // to something times 10 to -7. So we can't compare against 0
        if (Math::dot_product( Math::Vector(iter->normal), v.v()-iter->a.v() ) > 0.00001) {
            // This triangle can be "seen" from the new point, it needs to
            // be removed.
            push_edge( iter->a, iter->b );
            push_edge( iter->b, iter->c );
            push_edge( iter->c, iter->a );
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
void Minkowski_polytope::push_edge( const Minkowski_vector& a, const Minkowski_vector& b )
{
    for (auto iter=m_edges.cbegin(), end=m_edges.cend(); iter!=end; ++iter) {
        if (iter->a.v() == b.v() && iter->b.v() == a.v()) {
            m_edges.erase( iter );
            return;
        }
    }
    m_edges.emplace_back( a, b );
}

}}

