#include "OpenGLAttributes.h"
#include "Color.h"

#include <assert.h>
#include <stdexcept>

using Dubious::Renderer::OpenGLAttributes;
using Dubious::Renderer::Color;
using Dubious::Math::Point;

////////////////////////////////////////////////////////////////////////////////////
OpenGLAttributes::OpenGLAttributes( int BitField, bool Restore )
    : m_Restore( Restore )
    , m_GroupBits( BitField )
{
    if( m_Restore )
        glPushAttrib( m_GroupBits );
}

////////////////////////////////////////////////////////////////////////////////////
OpenGLAttributes::~OpenGLAttributes()
{
    if( m_Restore )
        glPopAttrib();
}

////////////////////////////////////////////////////////////////////////////////////
void OpenGLAttributes::Clear(int GroupBitField)
{
    glClear( GroupBitField );
}

////////////////////////////////////////////////////////////////////////////////////
void OpenGLAttributes::Enable( AttrBits BitField ) const
{
#ifdef _DEBUG
    VerifyBits( BitField );
#endif
    glEnable( BitField );
}

////////////////////////////////////////////////////////////////////////////////////
void OpenGLAttributes::Disable( AttrBits BitField ) const
{
#ifdef _DEBUG
    VerifyBits( BitField );
#endif
    glDisable( BitField );
}

////////////////////////////////////////////////////////////////////////////////////
void OpenGLAttributes::LightPosition( int Light, const Point& Position ) const
{
    float LightParams[4];
    LightParams[0] = Position.X();
    LightParams[1] = Position.Y();
    LightParams[2] = Position.Z();
    LightParams[3] = 0.0f;	// Directional light (like the sun)
    glLightfv( Light, GL_POSITION, LightParams );

}

////////////////////////////////////////////////////////////////////////////////////
void OpenGLAttributes::LightColor( int Light, int Type, const Color& Color ) const
{
    float LightParams[4];
    LightParams[0] = Color.Red();
    LightParams[1] = Color.Green();
    LightParams[2] = Color.Blue();
    LightParams[3] = Color.Alpha();
    glLightfv( Light, Type, LightParams );
}

////////////////////////////////////////////////////////////////////////////////////
void OpenGLAttributes::VerifyBits( AttrBits BitField ) const
{
    if( m_GroupBits == ALL_ATTRIB_BITS )
        return;
    if( !(m_GroupBits & ENABLE_BIT) ){
        if( BitField == TEXTURE_2D || BitField == BLEND ||
            BitField == LIGHTING   || BitField == SCISSOR_TEST ||
            BitField == STENCIL_TEST ){
            assert( false );
            throw std::runtime_error( "Bit Field doesn't match Group Bits" );
        }
    }
    if( !(m_GroupBits & DEPTH_BUFFER_BIT) ){
        if( BitField == DEPTH_TEST ){
            assert( false );
            throw std::runtime_error( "Bit Field doesn't match Group Bits" );
        }
    }
    if( !(m_GroupBits & COLOR_BUFFER_BIT) ){
    }
    if( !(m_GroupBits & LIGHTING_BIT) ){
        if( BitField == COLOR_MATERIAL ){
            assert( false );
            throw std::runtime_error( "Bit Field doesn't match Group Bits" );
        }
    }
    if( !(m_GroupBits & HINT_BIT) ){
        if( BitField == LINE_SMOOTH ){
            assert( false );
            throw std::runtime_error( "Bit Field doesn't match Group Bits" );
        }
    }
    if( !(m_GroupBits & POLYGON_BIT) ){
        if( BitField == CULL_FACE ){
            assert( false );
            throw std::runtime_error( "Bit Field doesn't match Group Bits" );
        }
    }
    if( !(m_GroupBits & TEXTURE_BIT) ){
    }
    if( !(m_GroupBits & PIXEL_MODE_BIT) ){
    }
}


