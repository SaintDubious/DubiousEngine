#ifndef INCLUDED_PHYSICS_CONSTRAINTSTRATEGYSIMPLE
#define INCLUDED_PHYSICS_CONSTRAINTSTRATEGYSIMPLE

#include "Constraint_strategy.h"
#include "Constraint_solver.h"

namespace Dubious {
namespace Physics {

/// @brief Simple, single threaded Constraint Solver
///
/// The simplest version of a constraint solver. This just runs
/// everything on a single thread. Good for learning what's going
/// on, but not terrible efficient.
class Constraint_strategy_simple : public Constraint_strategy {
public:
    /// @brief Constructor
    ///
    /// @param time_step - [in] how much time elapses per run
    /// @param beta - [in] See Arena::Constraint_solver_settings::beta
    /// @param cor - [in] See Arena::Constraint_solver_settings::coefficient_of_restitution
    /// @param slop - [in] See Arena::Constraint_solver_settings::slop
    Constraint_strategy_simple(float time_step, float beta, float cor, float slop);

    /// @brief Destructor
    ~Constraint_strategy_simple();

    /// @brief Single Threaded warm_start
    ///
    /// See Constraint_strategy::warm_start
    virtual void warm_start(std::map<Physics_object_pair, Contact_manifold>& manifolds);

    /// @brief Single Threaded solve
    ///
    /// See Constraint_strategy::solve
    virtual void solve(int iterations, std::map<Physics_object_pair, Contact_manifold>& manifolds);

private:
    Constraint_solver m_constraint_solver;
};

}  // namespace Physics
}  // namespace Dubious

#endif
