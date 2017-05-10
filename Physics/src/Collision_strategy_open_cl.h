#ifndef INCLUDED_PHYSICS_COLLISIONSTRATEGYOPENCL
#define INCLUDED_PHYSICS_COLLISIONSTRATEGYOPENCL

#include "Collision_strategy.h"
#include "Collision_solver.h"
#include "Open_cl.h"

#include <set>
#include <mutex>

namespace Dubious {
namespace Physics {

class Physics_object;
class Contact_manifold;

/// @brief Collision Strategy using OpenCL
///
/// As the name implies, it performs the collision detection using
/// OpenCL. The hardest to understand, but early tests show that 
/// it's significantly faster then other approaches.
class Collision_strategy_open_cl : public Collision_strategy {
public: 
    /// @brief Constructor
    ///
    /// @param manifold_persistent_threshold - [in] see Arena::Settings
    /// @param collisions_per_thread - [in] see Arena::Settings
    /// @param cl_broadphase_work_group_size - [in] see Arena::Settings
    Collision_strategy_open_cl( float manifold_persistent_threshold, unsigned int collisions_per_thread, int cl_broadphase_work_group_size );

    /// @brief Destructor
    ~Collision_strategy_open_cl();

    Collision_strategy_open_cl( const Collision_strategy_open_cl& ) = delete;
    Collision_strategy_open_cl& operator=( const Collision_strategy_open_cl& ) = delete;

    /// @brief See Collision_strategy::find_contacts
    void find_contacts( const std::vector<std::shared_ptr<Physics_object>>& objects,
                        std::map<Physics_object_pair, Contact_manifold>& manifolds ) final;
private:
    Collision_solver    m_collision_solver;
    const float         m_manifold_persistent_threshold;
    const unsigned int  m_collisions_per_thread;
    const int           m_cl_broadphase_work_group_size;
    std::mutex          m_manifolds_mutex;
   
    cl_platform_id      m_platform_id;
    cl_device_id        m_device_id;
    cl_context          m_context;
    cl_command_queue    m_command_queue;
    cl_program          m_broad_phase_program;
    cl_kernel           m_broad_phase_inner_kernel;
    cl_kernel           m_broad_phase_outer_kernel;
    cl_mem              m_broad_phase_buffer_obj_a;
    cl_mem              m_broad_phase_buffer_obj_b;
    cl_mem              m_broad_phase_buffer_result;
    cl_float            *m_broad_phase_objects = nullptr;
    cl_char             *m_broad_phase_results = nullptr;

    std::set<Physics_object_pair> solve_collisions( std::vector<Physics_object_pair>&& inputs,
                                                    std::map<Physics_object_pair, Contact_manifold>& manifolds );
    std::vector<std::tuple<size_t,size_t>> openCL_broad_phase_inner( const std::vector<std::shared_ptr<Physics_object>>& objects, size_t offset, size_t length );
    std::vector<std::tuple<size_t,size_t>> openCL_broad_phase_outer( const std::vector<std::shared_ptr<Physics_object>>& objects, size_t offset_a, size_t offset_b, size_t length );

};

}
}

#endif

