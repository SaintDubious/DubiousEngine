#ifndef INCLUDED_PHYSICS_COLLISIONSTRATEGY
#define INCLUDED_PHYSICS_COLLISIONSTRATEGY

#include <vector>
#include <map>
#include <tuple>
#include <memory>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Physics {

class Physics_object;
class Contact_manifold;

/// @brief Interface for finding collisions between all objects
///
/// The job of the collision strategy is to take in all of the objects
/// in the world and find all contact points. The output from this 
/// needs to be contact manifolds between all colliding pairs. This
/// is a separate interface because there are a number of ways to
/// do this work. Currently I've implemented a single threaded
/// solver, a multi-threaded solver, and one that uses OpenCL.
class Collision_strategy {
public: 
    Collision_strategy( const Collision_strategy& ) = delete;
    Collision_strategy& operator=( const Collision_strategy& ) = delete;
    ~Collision_strategy() = default;

    // I try to avoid typedefs, but this one is so long and used 
    // so often that it becomes unruly
    typedef std::tuple<Physics_object*,Physics_object*> Physics_object_pair;

    /// @brief Find contacts between objects
    ///
    /// This is the main point of the Collision Strategy implementations.
    /// Given a vector of all of the objects in the universe, create (or 
    /// update) contacts manifolds for each colliding pair.
    /// @param objects - [in] All of the objects to compare
    /// @param manifolds - [in,out] contact information between each pair
    virtual void find_contacts( const std::vector<std::shared_ptr<Physics_object>>& objects,
                                std::map<Physics_object_pair, Contact_manifold>& manifolds ) = 0;
protected:
    Collision_strategy() = default;
};

}
}

#endif

