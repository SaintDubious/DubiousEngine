#ifndef INCLUDED_RENDERER_COLOR
#define INCLUDED_RENDERER_COLOR

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

/// @brief Simple color class.  
///
/// Stores RGBA.  Also contains a few predefined colors.
class Color
{
public:
    /// @brief Creates black
    Color();

    /// @brief Default copy constructor
    Color( const Color& ) = default;

    /// @brief Creates a color of the specified RGB with alpha=1
    /// @param R - [in] Red
    /// @param G - [in] Green
    /// @param B - [in] Blue
    Color( float R, float G, float B );

    /// @brief Creates a color of the specified RGBA
    /// @param R - [in] Red
    /// @param G - [in] Green
    /// @param B - [in] Blue
    /// @param A - [in] Alpha
    Color( float R, float G, float B, float A );

    /// @brief Default destructor
    ~Color() = default;

    /// @brief Default assignment operator
    Color& operator=( const Color& ) = default;

    /// @brief Access - read only access to red
    /// @returns red
    float               Red() const { return m_Red; }

    /// @brief Access - read only access to green
    /// @returns green
    float               Green() const { return m_Green; }

    /// @brief Access - read only access to blue
    /// @returns blue
    float               Blue() const { return m_Blue; }

    /// @brief Access - read only access to alpha
    /// @returns alpha
    float               Alpha() const { return m_Alpha; }

    static const Color  RED;
    static const Color  GREEN;
    static const Color  BLUE;
    static const Color  BLACK;
    static const Color  WHITE;
private:
    float               m_Red;
    float               m_Green;
    float               m_Blue;
    float               m_Alpha;
};

}
}

#endif
