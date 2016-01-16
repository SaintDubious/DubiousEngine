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
class Open_gl_attributes {
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
    Open_gl_attributes( int group_bit_field, bool push_stack );

    Open_gl_attributes( const Open_gl_attributes& ) = delete;

    /// @brief Destructor.  
    ///
    /// If PushStack was specified in the constructor
    ///	then this will pop the stack
    ~Open_gl_attributes();

    Open_gl_attributes& operator=( const Open_gl_attributes& ) = delete;

    /// @brief GroupBits describe the different grouping of attribute bits.
    enum Group_bits
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
    enum Attr_bits
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
    void                enable( Attr_bits bit_field ) const;

    /// @brief glDisable
    void                disable( Attr_bits bit_field ) const;

    /// @brief glClear
    static void         clear( int group_bit_field );

    /// @brief Sets the light position information.  
    /// @param Light - [in] LIGHT0
    /// @param Position - [in] For now I only do directional lights.
    void				light_position( int light, const Math::Point& position ) const;

    /// @brief Sets the light color
    /// @param Light - [in] LIGHT0
    /// @param Type - [in] GL_AMBIENT, GL_DIFFUSE, or GL_SPECULAR
    /// @param Color - [in] what color you want it to be
    void				light_color( int light, int type, const Renderer::Color& color ) const;

    /// @brief glDepthFunc
    static void			depth_func( int func )
    {
        glDepthFunc( func );
    }

    /// @brief glBlendFunc
    static void			blend_func( int s_factor, int d_factor )
    {
        glBlendFunc( s_factor, d_factor );
    }

    /// @brief glStencilFunc
    static void			stencil_func( int func, int ref, unsigned int mask )
    {
        glStencilFunc( func, ref, mask );
    }

    /// @brief glColorMask
    static void			color_mask( bool red, bool green, bool blue, bool alpha )
    {
        glColorMask( red, green, blue, alpha );
    }

    /// @brief glDepthMask
    static void			depth_mask( bool flag )
    {
        glDepthMask( flag );
    }

    /// @brief glStencilOp
    static void			stencil_op( int fail, int z_fail, int z_pass )
    {
        glStencilOp( fail, z_fail, z_pass );
    }

private:
    void                verify_bits( Attr_bits bit_field ) const;
    bool                m_restore;
    int                 m_group_bits;
};

}
}

#endif
