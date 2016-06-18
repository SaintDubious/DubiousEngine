#ifndef INCLUDED_PHYSICS_MINKOWSKI_POLYTOPE
#define INCLUDED_PHYSICS_MINKOWSKI_POLYTOPE

#include "Minkowski_simplex.h"

#include <Vector_math.h>

#include <list>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

/// @brief A Minkowski Polytope used in EPA
///
/// Constructed from the output of the Minkowski Simplex
/// created from the GJK collision detection. The Polytope keeps
/// track of the expanding object used in EPA
class Minkowski_polytope {
public:
    /// @brief Usable Constructor
    ///
    /// constructs the polytope from the simplex. If the simplex is not
    /// a tetrahedron, this will throw an exception
    /// @param simplex - [in] tetrahedron
    Minkowski_polytope( const Minkowski_simplex& simplex );

    Minkowski_polytope( const Minkowski_polytope& ) = delete;
    Minkowski_polytope& operator=( const Minkowski_polytope& ) = delete;


    /// @brief A surface triangle
    ///
    /// Represents a triangle on the Polytope. Created from three Minkowski
    /// vectors. Will throw an exception if it can't find the normal
    struct Triangle {
        /// @brief Usable Constructor
        Triangle( const Minkowski_vector& p1, const Minkowski_vector& p2, const Minkowski_vector& p3 )
            : a( p1 )
            , b( p2 )
            , c( p3 ) 
        {
            normal = Math::cross_product( b.v()-a.v(), c.v()-a.v() );
        }

        /// @brief default constructor
        ///
        /// Only used when creating a triangle that is about to be copied from a return
        /// value. Don't use this in the real world.
        Triangle() = default;

        Minkowski_vector  a;
        Minkowski_vector  b;
        Minkowski_vector  c;
        Math::Unit_vector normal;
    };

    /// @brief find the closest triangle to the origin
    ///
    /// Used in the EPA algorithm, we need to find the closest triangle
    /// to the origin. From there we will use that (if it's on the hull)
    /// or remove it and replace it with a new support point
    /// @returns the triangle and the distance to the triangle
    std::tuple<Triangle,float> find_closest_triangle();
    
    /// @brief Add the vector to the polytope
    ///
    /// This is where the bulk of the work is done. Finds out which surfaces
    /// can "see" this vector and removes them. It then creates new triangles
    /// from the edges of the hole and this new vector.
    /// @param v - [in] the new vector on the polytope
    void push_back( Minkowski_vector&& v );

private:

    struct Edge {
        Edge( const Minkowski_vector& p1, const Minkowski_vector& p2 )
            : a( p1 )
            , b( p2 )
        {}
        Minkowski_vector a;
        Minkowski_vector b;
    };

    void push_edge( const Edge& edge );

    std::list<Triangle> m_triangles;
    std::list<Edge> m_edges;
};

}}

#endif

