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
/// track of the expanding object used in EPA.
///
/// NOTE: This class is absolutely a hot spot when profiling. As 
/// such it uses a number of optimizations that I wouldn't usually
/// suggest.
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

        // Possible optimization:
        // The internal tests for this thing care only about which side of the
        // triangle it's on, so this doesn't need to be Unit_vector. The 
        // the DENSE PhysicsTimer case shows a 1/6 speed up when this is just
        // a Vector. However it is used outside for other things, so I'm not
        // sure if it's safe.
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

    // This function takes 2 points (and not an Edge) as an optimization.
    // This way we can defer Edge creation until we actually know we need
    // to push one onto the m_edges vector.
    void push_edge( const Minkowski_vector& a, const Minkowski_vector& b );

    // These are vectors and not lists. I profiled this and vector was faster
    std::vector<Triangle> m_triangles;
    std::vector<Edge> m_edges;
};

}}

#endif

