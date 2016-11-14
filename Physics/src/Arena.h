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

    /// @brief Physics settings
    ///
    /// I read somewhere that a physics engine is an endless selection
    /// of knobs to turn. This struct holds the knobs.
    struct Settings {
        Settings()
        {}

        Settings( float step, int iter, float b, float cor, float sl, 
                  float persistent )
            : step_size( step )
            , iterations( iter )
            , beta( b )
            , coefficient_of_restitution( cor )
            , slop( sl )
            , manifold_persistent_threshold( persistent )
        {}

        /// How long, in seconds, for an individual time step. The
        /// engine will always perform physics updates in discrete
        /// units of this much time. 1/60th is a good number
        const float     step_size = 0.0166666f;

        /// How many iterations the constraint solver will take 
        /// per time step.
        const int       iterations = 20;

        /// Beta affects how much force is applied when objects are 
        /// overlapping. The further the overlap, the more separating
        /// force is applied. If this is set too low, objects can 
        /// overlap and never push apart. Set it too high and 
        /// resting objects will jitter.  
        const float     beta = 0.03f;

        /// This fake force is applied proportionally to how fast 
        /// objects are overlapping. This can help stop fast objects
        /// from penetrating too far. However if it's turned up too
        /// high it will cause bouncing.
        const float     coefficient_of_restitution = 0.5f;

        /// This affects both beta and the coefficient of restitution.
        /// If objects are overlapping less then this, then neither
        /// of those forces will be applied. A little bit of slop 
        /// will allow some penetration, but a stabler system. Not
        /// enough slop and things will become unstable.
        const float     slop = 0.05f;

        /// When a point is being added to the contact manifold it
        /// needs to be tested against existing points to see if it
        /// is new, or is already in the manifold. If the distance 
        /// squared from an existing point to a new is less then this
        /// threshold then the new point will be considered the same
        /// as the old.
        const float     manifold_persistent_threshold = 0.05f;

    };

    /// @brief Constructor
    /// @param settings - [in] settings (see above)
    Arena( const Settings& settings );

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
    int                 m_velocity_iterations = 1;
    float               m_elapsed = 0.0f;
    const Settings      m_settings;

    std::vector<std::shared_ptr<Physics_object>> m_objects;
    std::map<std::tuple<std::shared_ptr<Physics_object>,
                        std::shared_ptr<Physics_object>>, 
             Contact_manifold> m_manifolds;
};

}}

#endif
