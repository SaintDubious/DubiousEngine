#include "Open_gl_commands.h"
#include "Color.h"

namespace Dubious {
namespace Renderer {

void Open_gl_commands::material( int light, const Color& c )
{
    float raw_colour[4];
    raw_colour[0] = c.red();
    raw_colour[1] = c.green();
    raw_colour[2] = c.blue();
    raw_colour[3] = c.alpha();
    glMaterialfv( GL_FRONT, light, raw_colour );
}

}}
