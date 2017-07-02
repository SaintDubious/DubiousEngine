#ifndef INCLUDED_RENDERER_OPENGLCOMMANDS
#define INCLUDED_RENDERER_OPENGLCOMMANDS

#include "Color.h"

#include <Vector.h>
#include <Point.h>

#include <SDL_opengl.h>
#include <gl/GLU.h>

namespace Dubious {
namespace Renderer {

/// @brief Collection of OpenGL commands
///
/// Not a class so much as an abstraction of the OpenGL commands.
///	This keeps all OpenGL Commands in one class, instead of spread
///	all throughout the system.  It also lets OpenGL work with my
///	own types a bit easier.
class Open_gl_commands {
public:
    /// @brief glTranslate
    /// @param v - [in] translate by this amount
    template <int T>
    static void translate(const Math::VectorT<T>& v)
    {
        glTranslatef(v.x(), v.y(), v.z());
    }

    /// @brief glClearColor
    static void clear_color(const Color& c)
    {
        glClearColor(c.red(), c.green(), c.blue(), c.alpha());
    }

    /// @brief glViewport
    static void viewport(int x, int y, int width, int height) { glViewport(x, y, width, height); }

    /// @brief gluPerspective
    static void perspective(float fovy, float aspect, float z_near, float z_far)
    {
        gluPerspective(fovy, aspect, z_near, z_far);
    }

    /// @brief gluLookAt
    static void look_at(const Math::Point& position, const Math::Point& target,
                        const Math::Vector& up)
    {
        gluLookAt(position.x(), position.y(), position.z(), target.x(), target.y(), target.z(),
                  up.x(), up.y(), up.z());
    }

    /// @brief glMultMatrix
    static void mult_matrix(float m[16]) { glMultMatrixf(m); }

    /// @brief glNewList
    static void new_list(unsigned int handle, int mode) { glNewList(handle, mode); }

    /// @brief glEndList
    static void end_list() { glEndList(); }

    /// @brief glCallList
    static void call_list(unsigned int handle) { glCallList(handle); }

    /// @brief glMaterial
    static void material(int light, const Color& c);

    /// @brief glCullFace
    static void cull_face(int face) { glCullFace(face); }

    /// @brief glPolygonMode
    static void polygon_mode(GLenum face, GLenum mode) { glPolygonMode(face, mode); }

    /// @brief glLineWidth
    static void line_width(float width) { glLineWidth(width); }
};

}  // namespace Renderer
}  // namespace Dubious

#endif
