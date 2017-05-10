#ifndef INCLUDED_PHYSICS_PHYSICSMODEL
#define INCLUDED_PHYSICS_PHYSICSMODEL

#include <Vector.h>

#include <vector>
#include <memory>

namespace Dubious {

namespace Utility { 
class Ac3d_file;
class Ac3d_model;
}

namespace Physics {

/// @brief A model represented to the Physics system
///
/// The model is a shared object that contains the data necessary
/// to represent the object to the Physics system. These are built to 
/// make collision detection faster and simpler. As such they flatten
/// everything out to Local_vectors 
class Physics_model {
public:
    Physics_model( const Physics_model& ) = delete;

    /// @brief Construct from AC3D Model
    ///
    /// Constructs a model from an AC3D File. 
    /// @param File - [in] The file object
    Physics_model( const Utility::Ac3d_file& File );

    Physics_model& operator=( const Physics_model& ) = delete;

    float               radius() const { return m_radius; }
    const std::vector<Math::Local_vector>& vectors() const { return m_vectors; }
    const std::vector<std::unique_ptr<Physics_model>>& kids() const { return m_kids; }

private:
    Physics_model() = default;
    void                construct( const Math::Local_vector& offset, const Utility::Ac3d_model& AC3DModel );

    float               m_radius = 0;
    std::vector<Math::Local_vector> m_vectors;
    std::vector<std::unique_ptr<Physics_model>> m_kids;

};

}}

#endif
