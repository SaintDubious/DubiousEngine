#ifndef INCLUDED_PHYSICS_PHYSICSOBJECT
#define INCLUDED_PHYSICS_PHYSICSOBJECT

#include <Coordinate_space.h>

#include <vector>
#include <memory>

//////////////////////////////////////////////////////////////
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
    Physics_object( const std::shared_ptr<Physics_model>& model, float mass );

    Physics_object( const Physics_object& ) = delete;

    Physics_object& operator=( const Physics_object& ) = delete;

    void                cache_collision_vectors() const;

    std::shared_ptr<Physics_model> model() const { return m_model; }

    /// @brief Accessor
    Math::Coordinate_space& coordinate_space() { return m_coordinate_space; }
    const Math::Coordinate_space& coordinate_space() const { return m_coordinate_space; }

    float               mass() const { return m_mass; }
    const Math::Vector& velocity() const { return m_velocity; }
    Math::Vector&       velocity() { return m_velocity; }
    const Math::Vector& force() const { return m_force; }
    Math::Vector&       force() { return m_force; }

    const std::vector<Math::Vector>& cached_collision_vectors() const { return m_cached_collision_vectors; }

private:
    std::shared_ptr<Physics_model> m_model;
    Math::Coordinate_space m_coordinate_space;
    mutable Math::Coordinate_space m_cached_coordinate_space;
    mutable std::vector<Math::Vector> m_cached_collision_vectors;

    // Linear Physics
    float               m_mass;
    Math::Vector        m_velocity;
    Math::Vector        m_force;

    // Angular Physics
};

}}

#endif
