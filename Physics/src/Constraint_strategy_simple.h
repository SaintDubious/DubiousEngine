#ifndef INCLUDED_PHYSICS_CONSTRAINTSTRATEGYSIMPLE
#define INCLUDED_PHYSICS_CONSTRAINTSTRATEGYSIMPLE

#include "Constraint_strategy.h"
#include "Constraint_solver.h"

namespace Dubious {
namespace Physics {

class Constraint_strategy_simple : public Constraint_strategy {
public:
    Constraint_strategy_simple( float time_step, float beta, float cor, float slop );
    ~Constraint_strategy_simple();

    virtual void warm_start( std::map<std::tuple<Physics_object*,Physics_object*>, Contact_manifold> &manifolds );
    virtual void solve( int iterations, std::map<std::tuple<Physics_object*,Physics_object*>, Contact_manifold> &manifolds );

private:
    Constraint_solver   m_constraint_solver;
};

}}

#endif


