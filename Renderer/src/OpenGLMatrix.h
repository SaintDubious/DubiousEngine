#ifndef INCLUDED_RENDERER_OPENGLMATRIX
#define INCLUDED_RENDERER_OPENGLMATRIX

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

/// @brief Open GL Matrix
///
/// This class is a safe way to make sure your OpenGL
///	Matrix pushes and pops line up.
class OpenGLMatrix
{
public:
    /// @brief calls push matrix
    OpenGLMatrix();

    OpenGLMatrix( const OpenGLMatrix& ) = delete;

    /// @brief calls pop matrix
    ~OpenGLMatrix();

    OpenGLMatrix& operator=( const OpenGLMatrix& ) = delete;

    /// @brief loads the identity matrix
    static void         LoadIdentity();

    /// @brief glMatrixMode
    static void			MatrixMode( int Mode );
};

}
}

#endif
