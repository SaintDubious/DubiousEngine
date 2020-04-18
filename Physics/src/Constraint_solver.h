#ifndef INCLUDED_PHYSICS_CONSTRAINTSOLVER
#define INCLUDED_PHYSICS_CONSTRAINTSOLVER

#include "Contact_manifold.h"

#include <Vector.h>

#include <vector>

namespace Dubious {
namespace Physics {

class Physics_object;

/// @brief Constraint Solver... solves constraints
///
/// This class is based entirely on the Sequential Impulse technique
/// described by Erin Catto
/// http://box2d.org/
/// And as pre-digested and explained by Ming-Lun "Allen" Chou
/// http://allenchou.net/game-physics-series/
class Constraint_solver {
public:
    /// @brief Constructor
    /// @param time_step - [in] See Arena::Constraint_solver_settings::step_size
    /// @param beta - [in] See Arena::Constraint_solver_settings::beta
    /// @param cor - [in] See Arena::Constraint_solver_settings::coefficient_of_restitution
    /// @param slop - [in] See Arena::Constraint_solver_settings::slop
    Constraint_solver(float time_step, float beta, float cor, float slop);

    Constraint_solver(const Constraint_solver&) = delete;
    Constraint_solver& operator=(const Constraint_solver&) = delete;

    /// @brief Re-apply forces from previous time step
    ///
    /// See the discussion by Allen Chou on his web page. The general idea is that
    /// often the previous time step's forces are mostly pertinent to the current time
    /// step. For exmaple a bunch of blocks at rest all have the same gravity every
    /// time step. So this just re-applies the same force. It's assumed that you've already scaled
    /// the impulses by using the Contact_manifold::scale_contact_impulses
    /// @param contact_manifold - [in] Up to 4 points that define the collision between the 2
    ///        objects stored in the manifold
    void warm_start(Contact_manifold& contact_manifold);

    /// @brief The heart of constraint solving
    ///
    /// Given two objects that are known to be colliding, and up to 4 points that are
    /// in the contact manifold. Figures out all of the forces at each point.
    /// @param contact_manifold - [in,out] Up to 4 points that define the collision.  Its
    ///        forces will be updated
    void solve(Contact_manifold& contact_manifold);

private:
    const float m_time_step;
    const float m_beta;
    const float m_coefficient_of_restitution;
    const float m_slop;
};

}  // namespace Physics
}  // namespace Dubious

#endif
