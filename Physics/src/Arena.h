#ifndef INCLUDED_PHYSICS_ARENA
#define INCLUDED_PHYSICS_ARENA

#include "Collision_strategy.h"
#include "Constraint_strategy.h"

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

    /// @brief Collision solver settings
    ///
    /// Settings specific to collision detection.
    struct Collision_solver_settings {

        enum class Strategy {
            SINGLE_THREADED,
            MULTI_THREADED,
            OPENCL
        };

        /// When a point is being added to the contact manifold it
        /// needs to be tested against existing points to see if it
        /// is new, or is already in the manifold. If the distance 
        /// squared from an existing point to a new is less then this
        /// threshold then the new point will be considered the same
        /// as the old.
        float           manifold_persistent_threshold = 0.05f;

        /// After broad phase collision detection we create a vector
        /// of potentially colliding pairs. If the number of these 
        /// pairs exceeds this number, the collision detection will
        /// be shunted off to a new thread. One new thread for every
        /// vector of pairs of the following size.
        /// This is specific to Collision_strategy_open_cl
        unsigned int    cl_collisions_per_thread = 100000;

        /// When using Collision_strategy_open_cl we need to know
        /// how many objects per global work group
        unsigned int    cl_collisions_work_group_size = 3200;

        /// When using Collision_strategy_multi_threaded we need to know
        /// how many objects per global work group
        unsigned int    mt_collisions_work_group_size = 1000;

        /// Which collision strategy should be used:
        /// SINGLE_THREADED -> Collision_strategy_simple
        /// MULTI_THREADED  -> Collision_strategy_multithreaded
        /// OPENCL          -> Collision_strategy_open_cl
        Strategy strategy = Strategy::SINGLE_THREADED;

    };

    /// @brief Constraint solver settings
    ///
    /// Settings for the constraint solver
    struct Constraint_solver_settings {

        enum class Strategy {
            SINGLE_THREADED,
            MULTI_THREADED
        };

        /// How long, in seconds, for an individual time step. The
        /// engine will always perform physics updates in discrete
        /// units of this much time. 1/60th is a good number
        float           step_size = 0.0166666f;

        /// How many iterations the constraint solver will take 
        /// per time step.
        int             iterations = 20;

        /// Beta affects how much force is applied when objects are 
        /// overlapping. The further the overlap, the more separating
        /// force is applied. If this is set too low, objects can 
        /// overlap and never push apart. Set it too high and 
        /// resting objects will jitter.  
        float           beta = 0.03f;

        /// This fake force is applied proportionally to how fast 
        /// objects are overlapping. This can help stop fast objects
        /// from penetrating too far. However if it's turned up too
        /// high it will cause bouncing.
        float           coefficient_of_restitution = 0.5f;

        /// This affects both beta and the coefficient of restitution.
        /// If objects are overlapping less then this, then neither
        /// of those forces will be applied. A little bit of slop 
        /// will allow some penetration, but a stabler system. Not
        /// enough slop and things will become unstable.
        float           slop = 0.05f;

        /// Which constraint strategy should be used:
        /// SINGLE_THREADED -> Constraint_strategy_simple
        /// MULTI_THREADED  -> Constraint_strategy_multithreaded
        Strategy strategy = Strategy::SINGLE_THREADED;
    };

    /// @brief Physics settings
    ///
    /// I read somewhere that a physics engine is an endless selection
    /// of knobs to turn. This struct holds the knobs.
    struct Settings {

        Settings()
        {}

        Settings( const Collision_solver_settings& col, const Constraint_solver_settings& con )
            : collision( col )
            , constraint( con )
        {}

        Collision_solver_settings collision;
        Constraint_solver_settings constraint;
    };

    /// @brief Constructor
    /// @param settings - [in] settings (see above)
    Arena( const Settings& settings );

    /// @brief Destructor
    ~Arena() = default;

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
    const std::map<std::tuple<Physics_object*,Physics_object*>, 
                   Contact_manifold>& manifolds() const { return m_manifolds; }
private:

    std::unique_ptr<Collision_strategy> m_collision_strategy;
    std::unique_ptr<Constraint_strategy> m_constraint_strategy;
    float               m_elapsed = 0.0f;
    const Settings      m_settings;

    // m_objects holds everything. The manifolds hold only pointers
    // and references to the m_objects. This is an optimization that got
    // me a 10% speedup. However it means if you ever remove anything from
    // m_objects, you will have to make sure to pull it out of m_manifolds
    // as well.
    std::vector<std::shared_ptr<Physics_object>> m_objects;
    std::map<std::tuple<Physics_object*,Physics_object*>, Contact_manifold> m_manifolds;
};

}}

#endif
