#ifndef INCLUDED_RENDERER_CAMERA
#define INCLUDED_RENDERER_CAMERA

#include <CoordinateSpace.h>

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
class Camera
{
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
	Camera( int X, int Y, int Width, int Height, float FOVY );

    Camera( const Camera& Other ) = delete;
    
    /// @brief Default destructor
	~Camera() = default;

    Camera&             operator=( const Camera& Other ) = delete;
    
    /// @brief Prepare Camera for rendering
	///
	/// This function should be the first thing called during the 
	///	drawing function.  It applies the camera information to the
	///	OpenGL buffers.
	void                ReadyForPhoto() const;

	/// @brief Unroject a 2D point
	///
	/// Takes the given 2D point and converts it into a point in
	/// 3D space.
	/// @param P - [in] Point to convert
	/// @returns The resulting vector
	Math::Point         Unproject( const std::pair<int,int>& P ) const;

	/// @brief Accessor 
	///
	/// Returns the coordinate space.  See above for the
	///	difference between 1st and 3rd person view
	/// @returns coordinate space reference
	Math::CoordinateSpace& CoordinateSpace() { return m_CoordinateSpace; }

	/// @brief Accessor 
	///
	/// Returns the ZAxisOffset.   See above for the
	///	difference between 1st and 3rd person view
	/// @returns ZAxis Offset reference
	float&				ZAxisOffset() { return m_ZAxisOffset; }

	/// @brief Accessor - gets the max depth
	/// @returns max depth reference
	float&              MaxDepth() { return m_MaxDepth; }

private:

    int                 m_ViewportX;
    int                 m_ViewportY;
    int                 m_ViewportWidth;
    int                 m_ViewportHeight;
    float               m_FOVY;
	Math::CoordinateSpace m_CoordinateSpace;
	float				m_ZAxisOffset;
	float               m_MaxDepth;
};

}
}

#endif
