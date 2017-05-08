#include "Constraint_strategy_simple.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

//////////////////////////////////////////////////////////////
Constraint_strategy_simple::Constraint_strategy_simple( float time_step, float beta, float cor, float slop )
    : m_constraint_solver( time_step, beta, cor, slop )
{
}

//////////////////////////////////////////////////////////////
Constraint_strategy_simple::~Constraint_strategy_simple()
{
}

//////////////////////////////////////////////////////////////
void Constraint_strategy_simple::warm_start( std::map<std::tuple<Physics_object*,Physics_object*>, Contact_manifold> &manifolds )
{
    for (auto &manifold : manifolds) {
        Physics_object* a = std::get<0>(manifold.first);
        Physics_object* b = std::get<1>(manifold.first);
        m_constraint_solver.warm_start( *a, *b, manifold.second );
    }
}

//////////////////////////////////////////////////////////////
void Constraint_strategy_simple::solve( int iterations, std::map<std::tuple<Physics_object*,Physics_object*>, Contact_manifold> &manifolds )
{
    for (int i=0; i<iterations; ++i) {
        for (auto &manifold : manifolds) {
            Physics_object* a = std::get<0>(manifold.first);
            Physics_object* b = std::get<1>(manifold.first);
            m_constraint_solver.solve( *a, *b, manifold.second );
        }
    }
}

}}
