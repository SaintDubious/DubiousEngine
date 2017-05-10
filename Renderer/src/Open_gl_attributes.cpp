#include "Open_gl_attributes.h"
#include "Color.h"

#include <assert.h>
#include <stdexcept>

namespace Dubious {
namespace Renderer {

Open_gl_attributes::Open_gl_attributes( int bit_field, bool restore )
    : m_restore( restore )
    , m_group_bits( bit_field )
{
    if( m_restore )
        glPushAttrib( m_group_bits );
}

Open_gl_attributes::~Open_gl_attributes()
{
    if( m_restore )
        glPopAttrib();
}

void Open_gl_attributes::clear(int group_bit_field)
{
    glClear( group_bit_field );
}

void Open_gl_attributes::enable( Attr_bits bit_field ) const
{
#ifdef _DEBUG
    verify_bits( bit_field );
#endif
    glEnable( bit_field );
}

void Open_gl_attributes::disable( Attr_bits bit_field ) const
{
#ifdef _DEBUG
    verify_bits( bit_field );
#endif
    glDisable( bit_field );
}

void Open_gl_attributes::light_position( int light, const Math::Point& position ) const
{
    float light_params[4];
    light_params[0] = position.x();
    light_params[1] = position.y();
    light_params[2] = position.z();
    light_params[3] = 0.0f;	// Directional light (like the sun)
    glLightfv( light, GL_POSITION, light_params );

}

void Open_gl_attributes::light_color( int light, int type, const Color& color ) const
{
    float light_params[4];
    light_params[0] = color.red();
    light_params[1] = color.green();
    light_params[2] = color.blue();
    light_params[3] = color.alpha();
    glLightfv( light, type, light_params );
}

void Open_gl_attributes::verify_bits( Attr_bits bit_field ) const
{
    if( m_group_bits == ALL_ATTRIB_BITS )
        return;
    if( !(m_group_bits & ENABLE_BIT) ){
        if( bit_field == TEXTURE_2D || bit_field == BLEND ||
            bit_field == LIGHTING   || bit_field == SCISSOR_TEST ||
            bit_field == STENCIL_TEST ){
            assert( false );
            throw std::runtime_error( "Bit Field doesn't match Group Bits" );
        }
    }
    if( !(m_group_bits & DEPTH_BUFFER_BIT) ){
        if( bit_field == DEPTH_TEST ){
            assert( false );
            throw std::runtime_error( "Bit Field doesn't match Group Bits" );
        }
    }
    if( !(m_group_bits & COLOR_BUFFER_BIT) ){
    }
    if( !(m_group_bits & LIGHTING_BIT) ){
        if( bit_field == COLOR_MATERIAL ){
            assert( false );
            throw std::runtime_error( "Bit Field doesn't match Group Bits" );
        }
    }
    if( !(m_group_bits & HINT_BIT) ){
        if( bit_field == LINE_SMOOTH ){
            assert( false );
            throw std::runtime_error( "Bit Field doesn't match Group Bits" );
        }
    }
    if( !(m_group_bits & POLYGON_BIT) ){
        if( bit_field == CULL_FACE ){
            assert( false );
            throw std::runtime_error( "Bit Field doesn't match Group Bits" );
        }
    }
    if( !(m_group_bits & TEXTURE_BIT) ){
    }
    if( !(m_group_bits & PIXEL_MODE_BIT) ){
    }
}

}}
