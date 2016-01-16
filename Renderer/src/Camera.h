#ifndef INCLUDED_RENDERER_CAMERA
#define INCLUDED_RENDERER_CAMERA

#include <Coordinate_space.h>

#include <memory>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

/// @brief Abstraction of a camera.  
///
/// Handles pointing the camera and
///	readying the scene for drawing.  This camera can work as a first
///	or third person camera.  If you set the ZAxisOffset to 0 then
///	the coordinate space is centered in the camera and any rotations
///	will turn the camera directly.  If ZAxisOffset is not 0 then it 
///	is the distance from the camera to the coordinate space along the
///	Z axis.  In this mode, rotations to the coordinate space will
///	circle the camera around the target
class Camera {
public:
    /// @brief Constructor 
    ///
    /// Creates a camera at position (0,0,0) looking
    ///	down the positive Z axis in first person mode.
    /// @param X - [in] the viewport X
    /// @param Y - [in] the viewport Y
    /// @param Width - [in] the viewport Width
    /// @param Height - [in] the viewport Height
    /// @param FOVY - [in] The Field Of View Y axis
    Camera( int x, int y, int width, int height, float fovy );

    Camera( const Camera& Other ) = delete;
    
    Camera&             operator=( const Camera& Other ) = delete;
    
    /// @brief Prepare Camera for rendering
    ///
    /// This function should be the first thing called during the 
    ///	drawing function.  It applies the camera information to the
    ///	OpenGL buffers.
    void                ready_for_photo() const;

    /// @brief Unroject a 2D point
    ///
    /// Takes the given 2D point and converts it into a point in
    /// 3D space.
    /// @param P - [in] Point to convert
    /// @returns The resulting vector
    Math::Point         unproject( const std::pair<int,int>& p ) const;

    /// @brief Accessor 
    ///
    /// Returns the coordinate space.  See above for the
    ///	difference between 1st and 3rd person view
    /// @returns coordinate space reference
    Math::Coordinate_space& coordinate_space() { return m_coordinate_space; }

    /// @brief Accessor 
    ///
    /// Returns the ZAxisOffset.   See above for the
    ///	difference between 1st and 3rd person view
    /// @returns ZAxis Offset reference
    float&				z_axis_offset() { return m_z_axis_offset; }

    /// @brief Accessor - gets the max depth
    /// @returns max depth reference
    float&              max_depth() { return m_max_depth; }

private:

    int                 m_viewport_x = 0;
    int                 m_viewport_y = 0;
    int                 m_viewport_width = 640;
    int                 m_viewport_height = 480;
    float               m_fovy = 90.0f;
    Math::Coordinate_space m_coordinate_space;
    float				m_z_axis_offset = 10.0f;
    float               m_max_depth = 100.0f;
};

}
}

#endif
