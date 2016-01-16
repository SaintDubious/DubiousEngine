#include "Camera.h"
#include "Open_gl_commands.h"
#include "Open_gl_matrix.h"

#include <tuple>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

//////////////////////////////////////////////////////////////
Camera::Camera( int x, int y, int width, int height, float fovy )
    : m_viewport_x( x )
    , m_viewport_y( y )
    , m_viewport_width( width )
    , m_viewport_height( height )
    , m_fovy( fovy )
    , m_max_depth( 500.0f )
{
}

//////////////////////////////////////////////////////////////
void Camera::ready_for_photo() const
{
    Open_gl_commands::clear_color( Color( 1.0f, 1.0f, 1.0f, 1.0f ) );
    Open_gl_commands::viewport( m_viewport_x, m_viewport_y, m_viewport_width, m_viewport_height );
    
    // projection
    Open_gl_matrix::matrix_mode( GL_PROJECTION );
    Open_gl_matrix::load_identity();
    float aspect_ratio = static_cast<float>(m_viewport_width) / static_cast<float>(m_viewport_height);
    Open_gl_commands::perspective( m_fovy, aspect_ratio, 1.0f, m_max_depth );

    // modelview
    Open_gl_matrix::matrix_mode( GL_MODELVIEW );
    Open_gl_matrix::load_identity();
    Math::Unit_vector y, z;
    std::tie( std::ignore, y, z) = m_coordinate_space.get_axes();
    Math::Point eye = m_coordinate_space.position();

    // If the m_ZAxisOffset is not equal to zero then the coordinate space of this
    // camera is situated in front of the camera (third person mode).  In this 
    // case we want to move the camera along the positive Z axis for the amount
    // specified by the Z Axis Offset
    if (!Math::equals( m_z_axis_offset, 0.0f )) {
        Math::Vector offset = z;
        offset = offset * m_z_axis_offset;
        eye = eye + offset;
    }
    Open_gl_commands::look_at( eye, eye-z, y );
}

//////////////////////////////////////////////////////////////
Math::Point Camera::unproject( const std::pair<int,int>& p ) const
{
    double x, y, z;
    double model_matrix[16];
    double projection_matrix[16];
    int viewport[4];
    glGetDoublev( GL_MODELVIEW_MATRIX, model_matrix );
    glGetDoublev( GL_PROJECTION_MATRIX, projection_matrix );
    glGetIntegerv( GL_VIEWPORT, viewport ); 

    gluUnProject( p.first, p.second, 0, model_matrix, projection_matrix, viewport, &x, &y, &z );
    
    return Math::Point( static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) );
}

}}