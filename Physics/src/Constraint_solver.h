#ifndef INCLUDED_PHYSICS_CONSTRAINTSOLVER
#define INCLUDED_PHYSICS_CONSTRAINTSOLVER

#include "Contact_manifold.h"

#include <Vector.h>

#include <vector>

//////////////////////////////////////////////////////////////
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
    Constraint_solver( float time_step, float beta, float cor, float slop );

    Constraint_solver( const Constraint_solver& ) = delete;
    Constraint_solver& operator=( const Constraint_solver& ) = delete;

    void                warm_start( Physics_object& a, Physics_object& b, Contact_manifold& contact_manifold );
    void                solve( Physics_object& a, Physics_object& b, Contact_manifold& contact_manifold );
private:
    
    const float         m_time_step;
    const float         m_beta;
    const float         m_coefficient_of_restitution;
    const float         m_slop;
};

}}

#endif
