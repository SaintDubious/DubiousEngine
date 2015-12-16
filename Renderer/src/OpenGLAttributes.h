#ifndef INCLUDED_RENDERER_OPENGLATTRIBUTES
#define INCLUDED_RENDERER_OPENGLATTRIBUTES

#include <Point.h>

#include <SDL_opengl.h>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

class Color;

/// @brief OpenGLAttributes
///
/// The Attributes object is meant to help make sure that you are
///	correctly setting bits.  You can also use it to push and pop the
///	attribute stack.  Mostly it's meant to check that you're setting
///	a bit that belongs to the correct group bit.
class OpenGLAttributes
{
public:
    /// @brief Constructor.  
    ///
    /// Specify the group bits and whether or not
    ///	you want to push the attribute stack.  If it is pushed then
    ///	the destructor will pop it.
    /// @param GroupBitField - [in] Group bits describe what kind of bits
    ///	you will be changing.  If you try to change bits that are not
    ///	part of the GroupBit it will generate an assertion in debug
    /// @param PushStack - [in] whether or not you want to push
    OpenGLAttributes( int GroupBitField, bool PushStack );

    OpenGLAttributes( const OpenGLAttributes& ) = delete;

    /// @brief Destructor.  
    ///
    /// If PushStack was specified in the constructor
    ///	then this will pop the stack
    ~OpenGLAttributes();

    OpenGLAttributes& operator=( const OpenGLAttributes& ) = delete;

    /// @brief GroupBits describe the different grouping of attribute bits.
    enum GroupBits
    {
        ENABLE_BIT			= GL_ENABLE_BIT,
        DEPTH_BUFFER_BIT	= GL_DEPTH_BUFFER_BIT,
        COLOR_BUFFER_BIT	= GL_COLOR_BUFFER_BIT,
        STENCIL_BUFFER_BIT	= GL_STENCIL_BUFFER_BIT,
        HINT_BIT			= GL_HINT_BIT,
        POLYGON_BIT			= GL_POLYGON_BIT,
        LIGHTING_BIT		= GL_LIGHTING_BIT,
        TEXTURE_BIT			= GL_TEXTURE_BIT,
        PIXEL_MODE_BIT		= GL_PIXEL_MODE_BIT,
        ALL_ATTRIB_BITS		= GL_ALL_ATTRIB_BITS
    };

    /// @brief the actual attribute bits
    enum AttrBits
    {
        TEXTURE_2D			= GL_TEXTURE_2D,
        BLEND				= GL_BLEND,
        LIGHTING			= GL_LIGHTING,
        LIGHT0				= GL_LIGHT0,
        SCISSOR_TEST		= GL_SCISSOR_TEST,
        DEPTH_TEST			= GL_DEPTH_TEST,
        STENCIL_TEST		= GL_STENCIL_TEST,
        LINE_SMOOTH			= GL_LINE_SMOOTH,
        CULL_FACE			= GL_CULL_FACE,
        COLOR_MATERIAL		= GL_COLOR_MATERIAL
    };
    
    /// @brief glEnable
    void                Enable( AttrBits BitField ) const;

    /// @brief glDisable
    void                Disable( AttrBits BitField ) const;

    /// @brief glClear
    static void         Clear( int GroupBitField );

    /// @brief Sets the light position information.  
    /// @param Light - [in] LIGHT0
    /// @param Position - [in] For now I only do directional lights.
    void				LightPosition( int Light, const Math::Point& Position ) const;

    /// @brief Sets the light color
    /// @param Light - [in] LIGHT0
    /// @param Type - [in] GL_AMBIENT, GL_DIFFUSE, or GL_SPECULAR
    /// @param Color - [in] what color you want it to be
    void				LightColor( int Light, int Type, const Renderer::Color& Color ) const;

    /// @brief glDepthFunc
    static void			DepthFunc( int Func )
    {
        glDepthFunc( Func );
    }

    /// @brief glBlendFunc
    static void			BlendFunc( int SFactor, int DFactor )
    {
        glBlendFunc( SFactor, DFactor );
    }

    /// @brief glStencilFunc
    static void			StencilFunc( int Func, int Ref, unsigned int Mask )
    {
        glStencilFunc( Func, Ref, Mask );
    }

    /// @brief glColorMask
    static void			ColorMask( bool Red, bool Green, bool Blue, bool Alpha )
    {
        glColorMask( Red, Green, Blue, Alpha );
    }

    /// @brief glDepthMask
    static void			DepthMask( bool Flag )
    {
        glDepthMask( Flag );
    }

    /// @brief glStencilOp
    static void			StencilOp( int Fail, int ZFail, int ZPass )
    {
        glStencilOp( Fail, ZFail, ZPass );
    }

private:
    void                VerifyBits( AttrBits BitField ) const;
    bool                m_Restore;
    int                 m_GroupBits;
};

}
}

#endif
