#ifndef INCLUDED_PHYSICS_CONSTRAINTSTRATEGYOPENCL
#define INCLUDED_PHYSICS_CONSTRAINTSTRATEGYOPENCL

#include "Constraint_strategy.h"
#include "Constraint_solver.h"
#include "Open_cl.h"

namespace Dubious {
namespace Physics {

/// @brief OpenCL based constraint solver
///
/// A Constraint solver that uses OpenCL. This one will probably be the least easy to
/// understand as it will probably have to rewrite the Constraint_solver in OpenCL
class Constraint_strategy_open_cl : public Constraint_strategy {
public:
    /// @brief Constructor
    ///
    /// @param time_step - [in] how much time elapses per run
    /// @param beta - [in] See Arena::Constraint_solver_settings::beta
    /// @param cor - [in] See Arena::Constraint_solver_settings::coefficient_of_restitution
    /// @param slop - [in] See Arena::Constraint_solver_settings::slop
    Constraint_strategy_open_cl(float time_step, float beta, float cor, float slop);

    /// @brief Destructor
    ~Constraint_strategy_open_cl();

    /// @brief OpenCL warm_start
    ///
    /// See Constraint_strategy::warm_start
    virtual void warm_start(std::map<Physics_object_pair, Contact_manifold>& manifolds);

    /// @brief OpenCL solve
    ///
    /// See Constraint_strategy::solve
    virtual void solve(int iterations, std::map<Physics_object_pair, Contact_manifold>& manifolds);

private:
    cl_platform_id   m_platform_id;
    cl_device_id     m_device_id;
    cl_context       m_context;
    cl_command_queue m_command_queue;
    cl_program       m_program;
    cl_kernel        m_warm_start_kernel;
    cl_kernel        m_solve_kernel;
};

}  // namespace Physics
}  // namespace Dubious

#endif
