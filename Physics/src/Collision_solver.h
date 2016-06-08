#ifndef INCLUDED_PHYSICS_COLLISIONSOLVER
#define INCLUDED_PHYSICS_COLLISIONSOLVER

#include <Vector.h>
#include <Point.h>

#include <vector>

//////////////////////////////////////////////////////////////
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
    /// @brief Default constructor
    Collision_solver() = default;

    Collision_solver( const Collision_solver& ) = delete;

    Collision_solver& operator=( const Collision_solver& ) = delete;

    struct Contact {
        Math::Point     contact_point;
        Math::Unit_vector normal;
        float           penetration_depth;
    };

    /// @brief find the intersection of 2 objects
    ///
    /// This is the main entry point to the collision solver.
    /// Given two objects, find out if they collide, and if so, 
    /// where. This is a work in progress.
    /// @param a - [in] the first object
    /// @param b - [in] the second object
    /// @returns true if they collide
    bool                intersection( const Physics_object& a, const Physics_object& b, std::vector<Contact>& contact_manifold );

};

}}

#endif
