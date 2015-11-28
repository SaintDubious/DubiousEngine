#include "OpenGLCommands.h"
#include "Color.h"

//////////////////////////////////////////////////////////////
using Dubious::Renderer::OpenGLCommands;
using Dubious::Renderer::Color;

//////////////////////////////////////////////////////////////
void OpenGLCommands::Material( int Light, const Color& C )
{
    float RawColour[4];
    RawColour[0] = C.Red();
    RawColour[1] = C.Green();
    RawColour[2] = C.Blue();
    RawColour[3] = C.Alpha();
    glMaterialfv( GL_FRONT, Light, RawColour );
}

