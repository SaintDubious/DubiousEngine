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
    Physics_object( const std::shared_ptr<Physics_model>& model );

    Physics_object( const Physics_object& ) = delete;

    Physics_object& operator=( const Physics_object& ) = delete;

    void                cache_collision_vectors() const;

    /// @brief Accessor
    Math::Coordinate_space& coordinate_space() { return m_coordinate_space; }

    const std::vector<Math::Vector>& cached_collision_vectors() const { return m_cached_collision_vectors; }

private:

    std::shared_ptr<Physics_model> m_model;
    Math::Coordinate_space m_coordinate_space;
    mutable Math::Coordinate_space m_cached_coordinate_space;
    mutable std::vector<Math::Vector> m_cached_collision_vectors;
};

}}

#endif
