#ifndef INCLUDED_RENDERER_OPENGLPRIMITIVE
#define INCLUDED_RENDERER_OPENGLPRIMITIVE

#include <SDL_opengl.h>

namespace Dubious {
namespace Math {
template <int T> class PointT;
template <int T> class Unit_vectorT;
}
namespace Renderer {

/// @brief OpenGL Primitives
///
/// Class for managing the drawing of OpenGL primitive objects.
///	Manages the calling of glBegin and glEnd for you
class Open_gl_primitive {
public:
    /// @brief The type of the primitive
    enum Primitive_type
    {
        TRIANGLE_STRIP	= GL_TRIANGLE_STRIP,
        TRIANGLES	= GL_TRIANGLES,
        LINE_STRIP	= GL_LINE_STRIP,
        LINE_LOOP	= GL_LINE_LOOP,
        LINES		= GL_LINES,
        POINTS		= GL_POINTS,
        QUADS		= GL_QUADS
    };

    /// @brief Constructor, calls glBegin
    /// @param TheType - [in] The type of the primitive
    Open_gl_primitive( Primitive_type type ) 
    {
        glBegin( type );
    }

    Open_gl_primitive( const Open_gl_primitive& ) = delete;

    /// @brief Destructor, calls glEnd
    ~Open_gl_primitive()
    {
        glEnd();
    }

    Open_gl_primitive& operator=( const Open_gl_primitive& ) = delete;

    /// @brief Calls glVertex
    /// @param V - [in] The vector you want to rander
    template <int T>
    void                vertex( const Math::PointT<T>& v ) const
    {
        glVertex3f( v.x(), v.y(), v.z() );
    }

    /// @brief Calls glNormal
    /// @param N - [in] The normal you want to rander
    template <int T>
    void                normal( const Math::Unit_vectorT<T>& n ) const
    {
        glNormal3f( n.x(), n.y(), n.z() );
    }
};

}
}

#endif
