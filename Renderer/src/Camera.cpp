#include "Camera.h"
#include "OpenGLCommands.h"
#include "OpenGLMatrix.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

//////////////////////////////////////////////////////////////
Camera::Camera( int X, int Y, int Width, int Height, float FOVY )
    : m_ViewportX( X )
    , m_ViewportY( Y )
    , m_ViewportWidth( Width )
    , m_ViewportHeight( Height )
    , m_FOVY( FOVY )
    , m_MaxDepth( 500.0f )
{
}

//////////////////////////////////////////////////////////////
void Camera::ReadyForPhoto() const
{
    OpenGLCommands::ClearColor( Color( 1.0f, 1.0f, 1.0f, 1.0f ) );
    OpenGLCommands::Viewport( m_ViewportX, m_ViewportY, m_ViewportWidth, m_ViewportHeight );
    
    // projection
    OpenGLMatrix::MatrixMode( GL_PROJECTION );
    OpenGLMatrix::LoadIdentity();
    float AspectRatio = static_cast<float>(m_ViewportWidth) / static_cast<float>(m_ViewportHeight);
    OpenGLCommands::Perspective( m_FOVY, AspectRatio, 1.0f, m_MaxDepth );

    // modelview
    OpenGLMatrix::MatrixMode( GL_MODELVIEW );
    OpenGLMatrix::LoadIdentity();
    Math::UnitVector X, Y, Z;
    m_CoordinateSpace.GetAxes( X, Y, Z );
    Math::Point Eye = m_CoordinateSpace.Position();

    // If the m_ZAxisOffset is not equal to zero then the coordinate space of this
    // camera is situated in front of the camera (third person mode).  In this 
    // case we want to move the camera along the positive Z axis for the amount
    // specified by the Z Axis Offset
    if (!Math::Equals( m_ZAxisOffset, 0.0f )) {
        Math::Vector Offset = Z;
        Offset = Offset * m_ZAxisOffset;
        Eye = Eye + Offset;
    }
    OpenGLCommands::LookAt( Eye, Eye-Z, Y );
}

//////////////////////////////////////////////////////////////
Math::Point Camera::Unproject( const std::pair<int,int>& P ) const
{
    double X, Y, Z;
    double ModelMatrix[16];
    double ProjectionMatrix[16];
    int Viewport[4];
    glGetDoublev( GL_MODELVIEW_MATRIX, ModelMatrix );
    glGetDoublev( GL_PROJECTION_MATRIX, ProjectionMatrix );
    glGetIntegerv( GL_VIEWPORT, Viewport ); 

    gluUnProject( P.first, P.second, 0, ModelMatrix, ProjectionMatrix, Viewport, &X, &Y, &Z );
    
    return Math::Point( static_cast<float>(X), static_cast<float>(Y), static_cast<float>(Z) );
}

}}