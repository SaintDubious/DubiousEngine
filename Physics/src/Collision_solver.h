#ifndef INCLUDED_PHYSICS_COLLISIONSOLVER
#define INCLUDED_PHYSICS_COLLISIONSOLVER

#include "Contact_manifold.h"

#include <vector>

namespace Dubious {
namespace Physics {

class Physics_object;

/// @brief Collision solver that implements GJK
///
/// Given two PhysicsObjects, figure out if they intersect. Uses
/// the GJK algorithm to find object intersection:
/// http://mollyrocket.com/849
/// http://hacktank.net/blog/?p=93
/// As well as the EPA algorithm to find the contact point and normal:
/// http://hacktank.net/blog/?p=119
/// http://allenchou.net/2013/12/game-physics-contact-generation-epa/
/// http://stackoverflow.com/questions/31764305/im-implementing-the-expanding-polytope-algorithm-and-i-am-unsure-how-to-deduce
class Collision_solver {
public:
    /// @brief Constructor
    /// @param greedy_manifold - [in] sets whether this will try to find as many
    ///                          contacts as possible when finding intersections
    Collision_solver(bool greedy_manifold);

    Collision_solver(const Collision_solver&) = delete;
    Collision_solver& operator=(const Collision_solver&) = delete;

    /// @brief find the intersection of 2 objects
    ///
    /// This is the main entry point to the collision solver.
    /// Given two objects, find out if they collide, and if so,
    /// where. This is a work in progress.
    /// @param a - [in] the first object
    /// @param b - [in] the second object
    /// @param contacts - [out] contact information
    /// @returns true if they collide
    bool intersection(const Physics_object& a, const Physics_object& b,
                      std::vector<Contact_manifold::Contact>& contacts) const;

    /// @brief cheap and cheerful intersection test
    ///
    /// This is a cheap and very rough intersection test. It only
    /// looks to see if the radius of the 2 objects added is more
    /// than the distance between them. If it is then the objects
    /// might be touching
    /// @param a - [in] the first object
    /// @param b - [in] the second object
    /// @returns true if they might collide
    bool broad_phase_intersection(const Physics_object& a, const Physics_object& b) const;

private:
    bool m_greedy_manifold;
};

}  // namespace Physics
}  // namespace Dubious

#endif
