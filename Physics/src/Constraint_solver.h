#ifndef INCLUDED_PHYSICS_CONSTRAINTSOLVER
#define INCLUDED_PHYSICS_CONSTRAINTSOLVER

#include "Collision_solver.h"

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
    Constraint_solver( float time_step );

    Constraint_solver( const Constraint_solver& ) = delete;
    Constraint_solver& operator=( const Constraint_solver& ) = delete;

    struct Velocity_matrix {
        Math::Vector    a_linear;
        Math::Vector    a_angular;
        Math::Vector    b_linear;
        Math::Vector    b_angular;
    };
    Velocity_matrix solve( const Physics_object& a, const Physics_object& b, std::vector<Collision_solver::Contact>& contacts );
private:
    
    float               m_time_step;
};

}}

#endif
