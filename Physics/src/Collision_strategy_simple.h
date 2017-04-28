#ifndef INCLUDED_PHYSICS_COLLISIONSTRATEGYSIMPLE
#define INCLUDED_PHYSICS_COLLISIONSTRATEGYSIMPLE

#include "Collision_strategy.h"
#include "Collision_solver.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

class Physics_object;
class Contact_manifold;

/// @brief Simple, single threaded Collision Strategy
///
/// This Collision Strategy is as simple as it gets, all the work
/// in one thread, performed sequantially. This one won't be 
/// breaking any speed records, but it's the easiest to read and
/// learn from. Also it's the most likely to work in all cases 
class Collision_strategy_simple : public Collision_strategy {
public: 
    /// @brief Constructor
    /// 
    /// @param manifold_persistent_threshold - [in] see Arena::Settings
    Collision_strategy_simple( float manifold_persistent_threshold );

    /// @brief Destructor
    ~Collision_strategy_simple() = default;

    Collision_strategy_simple( const Collision_strategy& ) = delete;
    Collision_strategy_simple& operator=( const Collision_strategy& ) = delete;

    /// @brief See Collision_strategy::find_contacts
    void find_contacts( const std::vector<std::shared_ptr<Physics_object>>& m_objects,
                        std::map<Physics_object_pair, Contact_manifold>& manifolds ) final;
private:
    Collision_solver    m_collision_solver;
    const float         m_manifold_persistent_threshold;
};

}
}

#endif
