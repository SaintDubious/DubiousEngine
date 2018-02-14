#ifndef INCLUDED_PHYSICS_COLLISIONSTRATEGYMULTITHREADED
#define INCLUDED_PHYSICS_COLLISIONSTRATEGYMULTITHREADED

#include "Collision_strategy.h"
#include "Collision_solver.h"

#include <set>
#include <mutex>

namespace Dubious {
namespace Physics {

class Physics_object;
class Contact_manifold;

/// @brief Multi-threaded Collision Strategy
///
/// This one uses multiple CPU threads. It makes use of the new
/// C++11 async and future interface to spin up threads. This
/// should probably be the fallback option if OpenCL isn't available
class Collision_strategy_multi_threaded : public Collision_strategy {
public:
    /// @brief Constructor
    ///
    /// @param manifold_persistent_threshold - [in] see Arena::Settings
    /// @param manifold_movement_threshold - [in] see Arena::Settings
    /// @param greedy_manifold - [in] see Arena::Settings
    /// @param workgroup_size - [in] see Arena::Settings
    Collision_strategy_multi_threaded(float manifold_persistent_threshold,
                                      float manifold_movement_threshold, bool greedy_manifold,
                                      unsigned int workgroup_size);

    /// @brief Destructor
    ~Collision_strategy_multi_threaded() = default;

    Collision_strategy_multi_threaded(const Collision_strategy_multi_threaded&) = delete;
    Collision_strategy_multi_threaded& operator=(const Collision_strategy_multi_threaded&) = delete;

    /// @brief See Collision_strategy::find_contacts
    void find_contacts(const std::vector<std::shared_ptr<Physics_object>>& objects,
                       std::map<Physics_object_pair, Contact_manifold>&    manifolds) final;

private:
    Collision_solver   m_collision_solver;
    const float        m_manifold_persistent_threshold;
    const float        m_manifold_movement_threshold;
    const unsigned int m_workgroup_size;
    std::mutex         m_manifolds_mutex;

    std::set<Physics_object_pair> solve_inner(
        size_t start, size_t length, const std::vector<std::shared_ptr<Physics_object>>& objects,
        std::map<Physics_object_pair, Contact_manifold>& manifolds);

    std::set<Physics_object_pair> solve_outer(
        size_t a_start, size_t a_length, size_t b_start, size_t b_length,
        const std::vector<std::shared_ptr<Physics_object>>& objects,
        std::map<Physics_object_pair, Contact_manifold>&    manifolds);
};

}  // namespace Physics
}  // namespace Dubious

#endif
