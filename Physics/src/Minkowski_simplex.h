#ifndef INCLUDED_PHYSICS_MINKOWSKI_SIMPLEX
#define INCLUDED_PHYSICS_MINKOWSKI_SIMPLEX

#include "Minkowski_vector.h"

#include <vector>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

/// @brief The Minkowski Simplex used in GJK
///
/// The Simplex is the backbone of the GJK algorithm. It is comprised
/// of 2 to 4 points, representing a line, triangle, or tetrahedron.
class Minkowski_simplex {
public:
    /// @brief Constructor
    ///
    /// The simplex has to start with something. This creates
    /// an invalid simplex with only one point.
    /// @brief start - [in] the vector to start with
    Minkowski_simplex( Minkowski_vector&& start );

    /// @brief Add a Minkowski Vector to the simplex
    ///
    /// Simply puts the vector into the... vector. The
    /// heavy lifting is done in the nearest function
    /// @param v - [in] the Minkowski Vector to add
    void                push_back( Minkowski_vector&& v );

    /// @brief Find the next point on the simplex
    ///
    /// This is the bulk of the work of the Simplex. Finds
    /// the direction of the next point and corrects the
    /// Simplex internally.
    ///
    /// @returns true if there is a hope of collision along with the
    ///          new direction to search in
    std::tuple<bool,Math::Vector> build();

    /// @brief accessor
    const std::vector<Minkowski_vector>& v() const { return m_v; }

private:
    std::tuple<bool,Math::Vector> build_2();
    std::tuple<bool,Math::Vector> build_3();
    std::tuple<bool,Math::Vector> build_4();

    std::vector<Minkowski_vector> m_v;
};

}}

#endif
