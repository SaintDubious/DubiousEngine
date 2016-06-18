#ifndef INCLUDED_PHYSICS_MINKOWSKI_VECTOR
#define INCLUDED_PHYSICS_MINKOWSKI_VECTOR

#include <Vector.h>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

/// @brief A Minkowski Vector used in GJK and EPA
///
/// In the EPA algorithm we need to keep track of the Minkowski Vector,
/// the point on the Minkowski Simplex and Polytope, as well as the 
/// support points in global space that corresponds to this point. 
/// The bulk of GJK and EPA will only use the v() accessor, but once
/// the contact point is found (on the Minkowski Polytope), the support  
/// points will be needed to find that point in global space
class Minkowski_vector {
public:
    /// @brief Default constructor
    Minkowski_vector() = default;

    /// @brief Useful constructor
    ///
    /// Keeps track of the point on the Minkowski Simplex/Polytope
    /// as well as the support points on A and B that created it
    /// @param v - [in] point on the Simplex/Polytope
    /// @param support_a - [in] support from object A
    /// @param support_b - [in] support from object B
    Minkowski_vector( const Math::Vector& v, const Math::Vector& support_a, const Math::Vector& support_b )
        : m_v( v )
        , m_support_a( support_a )
        , m_support_b( support_b )
    {}

    /// @brief Accessor
    ///
    /// This is the most useful accessor as most of the collision
    /// point finding deals with this point
    const Math::Vector& v() const { return m_v; }

    /// @brief Accessor for support A
    const Math::Vector& support_a() const { return m_support_a; }

    /// @brief Accessor for support B
    const Math::Vector& support_b() const { return m_support_b; }

private:
    Math::Vector m_v;
    Math::Vector m_support_a;
    Math::Vector m_support_b;
};

}}

#endif

