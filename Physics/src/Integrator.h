#ifndef INCLUDED_PHYSICS_INTEGRATOR
#define INCLUDED_PHYSICS_INTEGRATOR

#include <vector>
#include <memory>

namespace Physics_test { class Integrator_test; }

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

class Physics_object;

/// @brief Perfrom RK4 integration. 
///
/// RK4 is an integration technique to take a physical object, apply a
/// force, and derive a new velocity and position. It works for both
/// linear and angular motion. It's probably overkill for this engine
/// as the main benefit seems to be that it can handle a force that is
/// changing over a given timestep, whereas I am envisioning a consistent
/// force over any given update loop. Still, it's cool and was fun to
/// implement. Check out this for information:
///  http://gafferongames.com/game-physics/integration-basics/
class Integrator {
public:
    /// @brief Constructor
    ///
    /// The only constructor for this object. Takes in the fixed size of
    /// the timestep. While each update may not be exactly of this size,
    /// internally it is only performing integration in discrete units
    /// of this size
    /// @param step_size - [in] the time step in seconds
    Integrator( float step_size );

    /// @brief Main entry point for physics updates
    ///
    /// Given the list of objects and how much time has passed since the
    /// last call to update, find the new velocities and positions and
    /// update the list of passed in objects.
    /// @param objects - [in,out] objects to be updated. NOT CONST
    /// @param elapsed - [in] time since last call, in seconds
    void                update( std::vector<std::shared_ptr<Physics_object>>& objects, float elapsed ); 

private:
    friend class Physics_test::Integrator_test;

    void                integrate_linear( Physics_object& physics_object, float dt ) const;
    void                integrate_angular( Physics_object& physics_object, float dt ) const;

    float               m_step_size;
    float               m_elapsed = 0.0f;
};

}}

#endif
