#ifndef INCLUDED_PHYSICS_COLLISIONSOLVER
#define INCLUDED_PHYSICS_COLLISIONSOLVER

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

class PhysicsObject;

/// @brief Collision solver that implements GJK
///
/// Given two PhysicsObjects, figure out if they intersect. Uses
/// the GJK algorithm:
/// http://mollyrocket.com/849
class CollisionSolver
{
public:
    /// @brief Default constructor
    CollisionSolver() = default;

    CollisionSolver( const CollisionSolver& ) = delete;

    /// @brief Default destructor
    ~CollisionSolver() = default;

    CollisionSolver& operator=( const CollisionSolver& ) = delete;

    bool                Intersection( const PhysicsObject& A, const PhysicsObject& B );

private:

};

}}

#endif
