#ifndef INCLUDED_PHYSICS_PHYSICSMODEL
#define INCLUDED_PHYSICS_PHYSICSMODEL

#include <Point.h>

#include <vector>
#include <memory>

//////////////////////////////////////////////////////////////
namespace Dubious {

namespace Utility { 
class Ac3d_file;
class Ac3d_model;
}

namespace Physics {

/// @brief A model represented to the Physics system
///
/// The model is a shared object that contains the data necessary
/// to represent the object to the Physics system
class Physics_model {
public:
    Physics_model( const Physics_model& ) = delete;

    /// @brief Construct from AC3D Model
    ///
    /// Constructs a model from an AC3D File. 
    /// @param File - [in] The file object
    Physics_model( const Utility::Ac3d_file& File );

    /// @brief Construct a cube
    ///
    /// Builds a cube of the given dimensions with the origin
    /// centered.
    /// @param Dimensions - [in] The Dimensions
    Physics_model( const Math::Triple& Dimensions );

    Physics_model& operator=( const Physics_model& ) = delete;

    const Math::Local_point& offset() const { return m_offset; }
    const std::vector<Math::Local_point>& points() const { return m_points; }
    const std::vector<std::unique_ptr<Physics_model>>& kids() const { return m_kids; }

private:
    Physics_model() = default;
    void                construct( const Utility::Ac3d_model& AC3DModel );


    Math::Local_point	m_offset;
    std::vector<Math::Local_point> m_points;
    std::vector<std::unique_ptr<Physics_model>> m_kids;

};

}}

#endif
