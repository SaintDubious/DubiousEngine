#ifndef INCLUDED_RENDERER_COLOR
#define INCLUDED_RENDERER_COLOR

#include <Utils.h>

namespace Dubious {
namespace Renderer {

/// @brief Simple color class.
///
/// Stores RGBA.  Also contains a few predefined colors.
class Color {
public:
    /// @brief default constructor. Makes black
    Color() = default;

    /// @brief Creates a color of the specified RGB with alpha=1
    /// @param R - [in] Red
    /// @param G - [in] Green
    /// @param B - [in] Blue
    Color(float r, float g, float b);

    /// @brief Creates a color of the specified RGBA
    /// @param R - [in] Red
    /// @param G - [in] Green
    /// @param B - [in] Blue
    /// @param A - [in] Alpha
    Color(float r, float g, float b, float a);

    /// @brief Access - read only access to red
    /// @returns red
    float red() const { return m_red; }

    /// @brief Access - read only access to green
    /// @returns green
    float green() const { return m_green; }

    /// @brief Access - read only access to blue
    /// @returns blue
    float blue() const { return m_blue; }

    /// @brief Access - read only access to alpha
    /// @returns alpha
    float alpha() const { return m_alpha; }

    static const Color RED;
    static const Color GREEN;
    static const Color BLUE;
    static const Color BLACK;
    static const Color WHITE;

private:
    float m_red   = 0.0f;
    float m_green = 0.0f;
    float m_blue  = 0.0f;
    float m_alpha = 1.0f;
};

inline bool
operator==(const Color& a, const Color& b)
{
    return Math::equals(a.red(), b.red()) && Math::equals(a.green(), b.green()) &&
           Math::equals(a.blue(), b.blue()) && Math::equals(a.alpha(), b.alpha());
}

}  // namespace Renderer
}  // namespace Dubious

#endif
