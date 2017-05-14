#ifndef INCLUDED_PHYSICS_CONSTRAINTSTRATEGYMULTITHREADED
#define INCLUDED_PHYSICS_CONSTRAINTSTRATEGYMULTITHREADED

#include "Constraint_strategy.h"
#include "Constraint_solver.h"

namespace Dubious {
namespace Physics {

/// @brief Multi-threaded constraint strategy
///
/// Runs the constraint solver on multiple threads. Uses C++11
/// futures and async. Not terribly difficult to understand, and
/// definitely faster then the Simple version.
class Constraint_strategy_multi_threaded : public Constraint_strategy {
public:
    /// @brief Constructor
    ///
    /// @param time_step - [in] how much time elapses per run
    /// @param beta - [in] See Arena::Constraint_solver_settings::beta
    /// @param cor - [in] See Arena::Constraint_solver_settings::coefficient_of_restitution
    /// @param slop - [in] See Arena::Constraint_solver_settings::slop
    Constraint_strategy_multi_threaded(float time_step, float beta, float cor, float slop);

    /// @brief Destructor
    ~Constraint_strategy_multi_threaded();

    /// @brief Multi Threaded warm_start
    ///
    /// See Constraint_strategy::warm_start
    virtual void warm_start(std::map<Physics_object_pair, Contact_manifold>& manifolds);

    /// @brief Multi Threaded solve
    ///
    /// See Constraint_strategy::solve
    virtual void solve(int iterations, std::map<Physics_object_pair, Contact_manifold>& manifolds);

private:
    struct Manifold_info {
        Physics_object*   a;
        Physics_object*   b;
        Contact_manifold* manifold;
    };

    void warm_start_internal(std::vector<Manifold_info>&& manifolds);
    void solve_internal(std::vector<Manifold_info>&& manifolds);

    Constraint_solver m_constraint_solver;
};

}  // namespace Physics
}  // namespace Dubious

#endif
