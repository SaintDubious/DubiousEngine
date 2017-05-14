#ifndef INCLUDED_PHYSICS_CONSTRAINT_STRATEGY_H
#define INCLUDED_PHYSICS_CONSTRAINT_STRATEGY_H

#include "Contact_manifold.h"

#include <map>

namespace Dubious {
namespace Physics {

class Physics_object;

/// @brief Interface for using different approaches to constraing solving
///
/// Similar to the Collision_strategy interface, this interface is used to
/// mask actual implementations of the constraint solver. In all cases, the
/// job of the constraing solver is to take colliding pairs of objects and
/// a contact manifold, and determing how the objects should move in order
/// to resolve any overlaps. This strategy will be implemented as a plain,
/// single threaded strategy, a multi-threaded one, and one using OpenCL
class Constraint_strategy {
public:
    Constraint_strategy(const Constraint_strategy&) = delete;
    Constraint_strategy& operator=(const Constraint_strategy&) = delete;

    /// @brief Destructor
    ~Constraint_strategy() = default;

    // I try to avoid typedefs, but this one is so long and used
    // so often that it becomes unruly
    typedef std::tuple<Physics_object*, Physics_object*> Physics_object_pair;

    /// @brief Apply the last time step's forces to this one
    ///
    /// This makes the assumption that the forces on this time step
    /// will be pretty much the same as last time step. This makes sense
    /// for things like resting blocks with gravity. Assume the forces
    /// will be the same and just re-apply them. This makes the solving
    /// step much simpler.
    /// @param manifolds - [in,out] pairs of objects and their contact manifolds
    virtual void warm_start(std::map<Physics_object_pair, Contact_manifold>& manifolds) = 0;

    /// @brief Solve the constraints
    ///
    /// Every object pair in the map has a contact manifold. Determine how to
    /// apply forces to the objects such that they will move apart and no longer
    /// overlap. Contains the crucial bits of collision resolution
    /// @param iterations - [in] how many times to run the resolution step
    /// @param manifolds - [in,out] pairs of objects and their contact manifolds
    virtual void solve(int                                              iterations,
                       std::map<Physics_object_pair, Contact_manifold>& manifolds) = 0;

protected:
    Constraint_strategy() = default;
};

}  // namespace Physics
}  // namespace Dubious

#endif
