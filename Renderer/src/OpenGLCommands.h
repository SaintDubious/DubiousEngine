#ifndef INCLUDED_RENDERER_OPENGLCOMMANDS
#define INCLUDED_RENDERER_OPENGLCOMMANDS

#include "Color.h"
#include "Vector.h"
#include "Point.h"

#include <SDL_opengl.h>
#include <gl/GLU.h>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

/// @brief Collection of OpenGL commands
///
/// Not a class so much as an abstraction of the OpenGL commands.
///	This keeps all OpenGL Commands in one class, instead of spread
///	all throughout the system.  It also lets OpenGL work with my 
///	own types a bit easier.
class OpenGLCommands
{
public:
    /// @brief glTranslate
    /// @param V - [in] translate by this amount
    template <int T>
    static void         Translate( const Math::VectorT<T>& V )
    {
        glTranslatef( V.X(), V.Y(), V.Z() );
    }

    /// @brief glClearColor
    static void			ClearColor( const Color& C )
    {
        glClearColor( C.Red(), C.Green(), C.Blue(), C.Alpha() );
    }

    /// @brief glViewport
    template <int T>
    static void			Viewport( int X, int Y, int Width, int Height )
    {
        glViewport( X, Y, Width, Height );
    }

    /// @brief gluPerspective
    static void			Perspective( float FOVY, float Aspect, float Near, float Far )
    {
        gluPerspective( FOVY, Aspect, Near, Far );
    }

    /// @brief gluLookAt
    static void			LookAt( const Math::Point& Position, const Math::Point& Target,
                                const Math::Vector& Up )
    {
        gluLookAt( Position.X(), Position.Y(), Position.Z(),
                   Target.X(), Target.Y(), Target.Z(),
                   Up.X(), Up.Y(), Up.Z() );
    }

    /// @brief glMultMatrix
    static void			MultMatrix( float m[16] )
    {
        glMultMatrixf( m );
    }

    /// @bief glNewList
    static void			NewList( unsigned int Handle, int Mode )
    {
        glNewList( Handle, Mode );
    }

    /// @bief glEndList
    static void			EndList()
    {
        glEndList();
    }

    /// @bief glCallList
    static void			CallList( unsigned int Handle )
    {
        glCallList( Handle );
    }

    /// @brief glMaterial
    static void			Material( int Light, const Color& C );

    /// @brief glCullFace
    static void			CullFace( int Face )
    {
        glCullFace( Face );
    }
};

}
}

#endif
