#ifndef INCLUDED_PHYSICS_PHYSICSOBJECT
#define INCLUDED_PHYSICS_PHYSICSOBJECT

#include <Coordinate_space.h>

#include <vector>
#include <memory>

namespace Dubious {
namespace Physics {

class Physics_model;

/// @brief A Physics Object
///
/// The actual object that will be used in collision detection/response.
/// This one holds things like forces, velocities, etc. It's the basic
/// representation of something in Physics
class Physics_object {
public:
    /// @brief Constructor
    Physics_object(const std::shared_ptr<Physics_model>& model, float mass);

    Physics_object(const Physics_object&) = delete;

    Physics_object& operator=(const Physics_object&) = delete;

    /// @brief Model Accessor
    ///
    /// This function is used during broadphase collision detection to get the
    /// radius. As such it's called a LOT. In a simple, single threaded test
    /// when this returns a std::shared_ptr<Physics_model> the test ran in 2.4
    /// seconds. When changed to a const reference it ran in 0.76 seconds.
    /// Do not change this without serious profiling
    /// @returns the physics model
    const Physics_model& model() const { return *m_model.get(); }

    static const float STATIONARY;

    /// @brief Accessor
    Math::Coordinate_space&       coordinate_space() { return m_coordinate_space; }
    const Math::Coordinate_space& coordinate_space() const { return m_coordinate_space; }

    /// @brief Unique object id
    ///
    /// In the Arena object we need the manifolds to be be indexed on the pair of objects that
    /// created the manifold. I was using the object pointer, but this meant that the order of the
    /// manifolds would vary between runs (depending on pointer location). This meant each time I
    /// ran it I would get different behaviors. So in order to make each run of a scene stable (with
    /// specific time inputs) for debugging, I need stable ids. So I use this. Doesn't matter what
    /// the id is, as long as it's unique and assigned the same between runs.
    int& id() { return m_id; }

    // linear
    float               inverse_mass() const { return m_inverse_mass; }
    const Math::Vector& velocity() const { return m_velocity; }
    Math::Vector&       velocity() { return m_velocity; }
    const Math::Vector& force() const { return m_force; }
    Math::Vector&       force() { return m_force; }

    // angular
    float               inverse_moment_of_inertia() const { return m_inverse_moment_of_inertia; }
    const Math::Vector& angular_velocity() const { return m_angular_velocity; }
    Math::Vector&       angular_velocity() { return m_angular_velocity; }
    const Math::Vector& torque() const { return m_torque; }
    Math::Vector&       torque() { return m_torque; }

private:
    std::shared_ptr<Physics_model> m_model;
    Math::Coordinate_space         m_coordinate_space;
    int                            m_id;

    // Linear Physics
    float        m_inverse_mass;
    Math::Vector m_velocity;
    Math::Vector m_force;

    // Angular Physics
    float        m_inverse_moment_of_inertia;
    Math::Vector m_angular_velocity;
    Math::Vector m_torque;
};

}  // namespace Physics
}  // namespace Dubious

#endif
