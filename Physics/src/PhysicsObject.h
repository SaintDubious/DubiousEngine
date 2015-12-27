#ifndef INCLUDED_PHYSICS_PHYSICSOBJECT
#define INCLUDED_PHYSICS_PHYSICSOBJECT

#include <CoordinateSpace.h>

#include <vector>
#include <memory>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {
typedef std::vector<Vector> VectorVector;
}

namespace Physics {

class PhysicsModel;
typedef std::shared_ptr<PhysicsModel> PhysicsModelPtr;

/// @brief A Physics Object
///
/// The actual object that will be used in collision detection/response.
/// This one holds things like forces, velocities, etc. It's the basic
/// representation of something in Physics
class PhysicsObject
{
public:
    PhysicsObject() = delete;

    /// @brief Constructor
    PhysicsObject( PhysicsModelPtr pModel );

    PhysicsObject( const PhysicsObject& ) = delete;

    /// @brief Default destructor
    ~PhysicsObject() = default;

    PhysicsObject& operator=( const PhysicsObject& ) = delete;

    void                CacheCollisionVectors() const;

    /// @brief Accessor
    Math::CoordinateSpace& CoordinateSpace() { return m_CoordinateSpace; }

    const Math::VectorVector& CachedCollisionVectors() const { return m_CachedCollisionVectors; }

private:

    PhysicsModelPtr     m_pModel;
    Math::CoordinateSpace m_CoordinateSpace;
    mutable Math::CoordinateSpace m_CachedCoordinateSpace;
    mutable Math::VectorVector  m_CachedCollisionVectors;
};

}}

#endif
