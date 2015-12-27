#ifndef INCLUDED_PHYSICS_PHYSICSMODEL
#define INCLUDED_PHYSICS_PHYSICSMODEL

#include <Point.h>

#include <vector>
#include <memory>

//////////////////////////////////////////////////////////////
namespace Dubious {

namespace Utility { 
class AC3DFile;
class AC3DModel;
}

namespace Math {
struct Triple;
typedef std::vector<LocalPoint> LocalPointVector;
}

namespace Physics {

class PhysicsModel;
typedef std::shared_ptr<PhysicsModel> PhysicsModelPtr;
typedef std::vector<PhysicsModelPtr> PhysicsModelPtrVector;

/// @brief A model represented to the Physics system
///
/// The model is a shared object that contains the data necessary
/// to represent the object to the Physics system
class PhysicsModel
{
public:
    PhysicsModel( const PhysicsModel& ) = delete;

    /// @brief Construct from AC3D Model
    ///
    /// Constructs a model from an AC3D File. 
    /// @param File - [in] The file object
    PhysicsModel( const Utility::AC3DFile& File );

    /// @brief Construct a cube
    ///
    /// Builds a cube of the given dimensions with the origin
    /// centered.
    /// @param Dimensions - [in] The Dimensions
    PhysicsModel( const Math::Triple& Dimensions );

    /// @brief Default destructor
    ~PhysicsModel() = default;

    PhysicsModel& operator=( const PhysicsModel& ) = delete;

    const Math::LocalPoint& Offset() const { return m_Offset; }
    const Math::LocalPointVector& Points() const { return m_Points; }
    const PhysicsModelPtrVector& Kids() const { return m_Kids; }

private:
    PhysicsModel() = default;
    void                Construct( const Utility::AC3DModel& AC3DModel );


    Math::LocalPoint	m_Offset;
    Math::LocalPointVector m_Points;
    PhysicsModelPtrVector m_Kids;

};

}}

#endif
