#ifndef INCLUDED_PHYSICS_CONSTRAINTSTRATEGYMULTITHREADED
#define INCLUDED_PHYSICS_CONSTRAINTSTRATEGYMULTITHREADED

#include "Constraint_strategy.h"
#include "Constraint_solver.h"

namespace Dubious {
namespace Physics {

class Constraint_strategy_multi_threaded : public Constraint_strategy {
public:
    Constraint_strategy_multi_threaded( float time_step, float beta, float cor, float slop );
    ~Constraint_strategy_multi_threaded();

    virtual void warm_start( std::map<std::tuple<Physics_object*,Physics_object*>, Contact_manifold> &manifolds );
    virtual void solve( int iterations, std::map<std::tuple<Physics_object*,Physics_object*>, Contact_manifold> &manifolds );

private:
    struct Manifold_info {
        Physics_object* a;
        Physics_object* b;
        Contact_manifold* manifold;
    };

    void warm_start_internal( std::vector<Manifold_info> &&manifolds );
    void solve_internal( std::vector<Manifold_info> &&manifolds );

    Constraint_solver   m_constraint_solver;
};

}}

#endif


