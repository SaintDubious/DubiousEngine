#ifndef INCLUDED_RENDERER_OPENGLMATRIX
#define INCLUDED_RENDERER_OPENGLMATRIX

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

/// @brief Open GL Matrix
///
/// This class is a safe way to make sure your OpenGL
///	Matrix pushes and pops line up.
class Open_gl_matrix {
public:
    /// @brief calls push matrix
    Open_gl_matrix();

    Open_gl_matrix( const Open_gl_matrix& ) = delete;

    /// @brief calls pop matrix
    ~Open_gl_matrix();

    Open_gl_matrix& operator=( const Open_gl_matrix& ) = delete;

    /// @brief loads the identity matrix
    static void         load_identity();

    /// @brief glMatrixMode
    static void			matrix_mode( int mode );
};

}
}

#endif
