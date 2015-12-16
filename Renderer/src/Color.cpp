#include "Color.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

const Color Color::RED   ( 1.0f, 0,    0 );
const Color Color::GREEN ( 0,    1.0f, 0 );
const Color Color::BLUE  ( 0,    0,    1.0f );
const Color Color::BLACK ( 0,    0,    0 );
const Color Color::WHITE ( 1.0f, 1.0f, 1.0f );

//////////////////////////////////////////////////////////////
Color::Color()
    : Color( 0, 0, 0, 1.0f )
{
}

//////////////////////////////////////////////////////////////
Color::Color( float R, float G, float B )
    : Color( R, G, B, 1.0f )
{
}

//////////////////////////////////////////////////////////////
Color::Color( float R, float G, float B, float A )
    : m_Red( R )
    , m_Green( G )
    , m_Blue( B )
    , m_Alpha( A )
{
}

}}