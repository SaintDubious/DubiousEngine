#include "Color.h"

namespace Dubious {
namespace Renderer {

const Color Color::RED   ( 1.0f, 0,    0 );
const Color Color::GREEN ( 0,    1.0f, 0 );
const Color Color::BLUE  ( 0,    0,    1.0f );
const Color Color::BLACK ( 0,    0,    0 );
const Color Color::WHITE ( 1.0f, 1.0f, 1.0f );

Color::Color( float r, float g, float b )
    : Color( r, g, b, 1.0f )
{
}

Color::Color( float r, float g, float b, float a )
    : m_red( r )
    , m_green( g )
    , m_blue( b )
    , m_alpha( a )
{
}

}}