#ifndef INCLUDED_PHYSICS_COLLISIONSOLVER
#define INCLUDED_PHYSICS_COLLISIONSOLVER

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

class Physics_object;

/// @brief Collision solver that implements GJK
///
/// Given two PhysicsObjects, figure out if they intersect. Uses
/// the GJK algorithm:
/// http://mollyrocket.com/849
/// http://hacktank.net/blog/?p=93
class Collision_solver {
public:
    /// @brief Default constructor
    Collision_solver() = default;

    Collision_solver( const Collision_solver& ) = delete;

    Collision_solver& operator=( const Collision_solver& ) = delete;

    bool                intersection( const Physics_object& a, const Physics_object& b );

};

}}

#endif
