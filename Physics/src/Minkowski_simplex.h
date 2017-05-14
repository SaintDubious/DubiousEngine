#ifndef INCLUDED_PHYSICS_MINKOWSKI_SIMPLEX
#define INCLUDED_PHYSICS_MINKOWSKI_SIMPLEX

#include "Minkowski_vector.h"

#include <vector>

namespace Dubious {
namespace Physics {

/// @brief The Minkowski Simplex used in GJK
///
/// The Simplex is the backbone of the GJK algorithm. It is comprised
/// of 2 to 4 points, representing a line, triangle, or tetrahedron.
class Minkowski_simplex {
public:
    /// @brief Default Constructor
    Minkowski_simplex();

    /// @brief Constructor
    ///
    /// This creates an invalid simplex with only one point.
    /// @brief start - [in] the vector to start with
    Minkowski_simplex(const Minkowski_vector& start);

    /// @brief Add a Minkowski Vector to the simplex
    ///
    /// Simply puts the vector into the... vector. The
    /// heavy lifting is done in the build function
    /// @param v - [in] the Minkowski Vector to add
    void push_back(const Minkowski_vector& v);

    /// @brief Find the next point on the simplex
    ///
    /// This is the bulk of the work of the Simplex. Finds
    /// the direction of the next point and corrects the
    /// Simplex internally.
    ///
    /// @returns true if there is a hope of collision along with the
    ///          new direction to search in
    std::tuple<bool, Math::Vector> build();

    /// @brief accessors
    const Minkowski_vector* v() const { return m_v; }
    int                     size() const { return m_size; }

private:
    std::tuple<bool, Math::Vector> build_2();
    std::tuple<bool, Math::Vector> build_3();
    std::tuple<bool, Math::Vector> build_4();

    // Notice that I'm not using a std::vector for this one.
    // I profiled this using the PhysicsTimer test and found
    // that the allocations were a hot spot. Using an array
    // on the stack got me a 35% speedup.
    Minkowski_vector m_v[4];
    int              m_size;
};

}  // namespace Physics
}  // namespace Dubious

#endif
