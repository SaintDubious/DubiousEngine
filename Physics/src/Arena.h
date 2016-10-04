#ifndef INCLUDED_PHYSICS_ARENA
#define INCLUDED_PHYSICS_ARENA

#include "Collision_solver.h"
#include "Constraint_solver.h"

#include <vector>
#include <memory>
#include <map>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

class Physics_object;

/// @brief Container for all of the Physics things
///
/// Much as the Scene describes everything that exists in the
/// Renderer, the Arena describes everything in the Physics
/// world. This is the container for all of the physical things
/// that are going to be forced, collided, moved, etc. A game
/// will have one of these.
class Arena {
public:
    /// @brief Constructor
    /// @param step_size - [in] time of a step size
    Arena( float step_size );

    Arena( const Arena& ) = delete;
    Arena& operator=( const Arena& ) = delete;

    /// @brief Run the main physics loop
    ///
    /// This is the entry point to all of the physics. This will
    /// apply forces, move, collide, resolve, and blow up
    /// @param elapsed - [in] how much time has elapsed since the last run
    void                run_physics( float elapsed );

    /// @brief Add a physics object to the Arena
    /// @param obj - [in] the object to add
    void                push_back( std::shared_ptr<Physics_object> obj );

    /// @brief Manifold accessor
    ///
    /// Not sure if this is useful in any situation EXCEPT the one
    /// in which I'm trying to debug by drawing the contacts
    const std::map<std::tuple<std::shared_ptr<Physics_object>,
                              std::shared_ptr<Physics_object>>, 
                   Contact_manifold>& manifolds() const { return m_manifolds; }
private:

    Collision_solver    m_collision_solver;
    Constraint_solver   m_constraint_solver;
    float               m_step_size;
    float               m_elapsed = 0.0f;

    std::vector<std::shared_ptr<Physics_object>> m_objects;
    std::map<std::tuple<std::shared_ptr<Physics_object>,
                        std::shared_ptr<Physics_object>>, 
             Contact_manifold> m_manifolds;
};

}}

#endif
