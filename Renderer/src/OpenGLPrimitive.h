#ifndef INCLUDED_RENDERER_OPENGLPRIMITIVE
#define INCLUDED_RENDERER_OPENGLPRIMITIVE

#include <Point.h>
#include <UnitVector.h>

#include <SDL_opengl.h>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

/// @brief OpenGL Primitives
///
/// Class for managing the drawing of OpenGL primitive objects.
///	Manages the calling of glBegin and glEnd for you
class OpenGLPrimitive
{
public:
    /// @brief The type of the primitive
    enum PrimitiveType
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
    OpenGLPrimitive( PrimitiveType TheType ) 
    {
        glBegin( TheType );
    }

    OpenGLPrimitive( const OpenGLPrimitive& ) = delete;

    /// @brief Destructor, calls glEnd
    ~OpenGLPrimitive()
    {
        glEnd();
    }

    OpenGLPrimitive& operator=( const OpenGLPrimitive& ) = delete;

    /// @brief Calls glVertex
    /// @param V - [in] The vector you want to rander
    template <int T>
    void                Vertex( const Math::PointT<T>& V ) const
    {
        glVertex3f( V.X(), V.Y(), V.Z() );
    }

    /// @brief Calls glNormal
    /// @param N - [in] The normal you want to rander
    template <int T>
    void                Normal( const Math::UnitVector<T>& N ) const
    {
        glNormal3f( N.X(), N.Y(), N.Z() );
    }
};

}
}

#endif
