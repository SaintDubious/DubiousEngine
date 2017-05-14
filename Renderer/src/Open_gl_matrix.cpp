#include "Open_gl_matrix.h"

#include <SDL_opengl.h>

namespace Dubious {
namespace Renderer {

Open_gl_matrix::Open_gl_matrix()
{
    glPushMatrix();
}

Open_gl_matrix::~Open_gl_matrix()
{
    glPopMatrix();
}

void
Open_gl_matrix::load_identity()
{
    glLoadIdentity();
}

void
Open_gl_matrix::matrix_mode(int mode)
{
    glMatrixMode(mode);
}

}  // namespace Renderer
}  // namespace Dubious
