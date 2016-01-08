#ifndef INCLUDED_MATH_TRIPLE
#define INCLUDED_MATH_TRIPLE

#include "Utils.h"

#include <ostream>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

/// @brief Three floats
///
/// There are a fair number of Vector and 3D point types in a physics
/// engine. They all need a basic, underlying type of 3 floats. This
/// triple is meant to handle the really basic stuff, like addition,
/// copying, and equality. I'm making the data public because it's
/// not a real class, but a collection of helper functions wrapper
/// around three floats
struct Triple {
    /// @brief Default Constructor
    ///
    /// Initializes a Triple to 0,0,0
    Triple() = default;

    /// @brief Constructor
    ///
    /// Creates a triple with the passed in values
    /// @param x - [in] X component
    /// @param y - [in] Y component
    /// @param z - [in] Z component
    Triple(float x, float y, float z)
        : m_x(x)
        , m_y(y)
        , m_z(z)
    {}

    float               m_x = 0;
    float               m_y = 0;
    float               m_z = 0;
};

//////////////////////////////////////////////////////////////
inline bool operator==( const Triple& a, const Triple& b )
{
    return equals(a.m_x, b.m_x) &&
           equals(a.m_y, b.m_y) &&
           equals(a.m_z, b.m_z);
}

//////////////////////////////////////////////////////////////
inline bool operator!=(const Triple& a, const Triple& b)
{
    return !equals(a.m_x, b.m_x) ||
           !equals(a.m_y, b.m_y) ||
           !equals(a.m_z, b.m_z);
}

//////////////////////////////////////////////////////////////
inline Triple operator+(const Triple& a, const Triple& b)
{
    return Triple(a.m_x + b.m_x, a.m_y + b.m_y, a.m_z + b.m_z);
}

//////////////////////////////////////////////////////////////
inline Triple operator-(const Triple& a, const Triple& b)
{
    return Triple(a.m_x - b.m_x, a.m_y - b.m_y, a.m_z - b.m_z);
}

//////////////////////////////////////////////////////////////
inline std::ostream& operator<<(std::ostream& o, const Triple& a)
{
    o << "(" << a.m_x << ", " << a.m_y << ", " << a.m_z << ")";
    return o;
}

}
}

#endif