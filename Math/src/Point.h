#ifndef INCLUDED_MATH_POINT
#define INCLUDED_MATH_POINT

#include "Triple.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

/// @brief A 3D Point
///
/// Represents a 3D point. This is different from a Vector in that
/// it is not a direction, it is simply a point in space. It is
/// a template only so that we can create Points intended for different
/// uses that the compiler can enforce
template <int>
class PointT
{
public:
    /// @brief Default Constructor
    ///
    /// Creates a Point at 0,0,0
    PointT()
    {}

    /// @brief Constructor
    ///
    /// Creates the point at the coordinate specified
    /// @param X - [in] X component
    /// @param Y - [in] Y component
    /// @param Z - [in] Z component
    PointT( float X, float Y, float Z )
        : m_Coords( X, Y, Z )
    {}
    
    /// @brief Default copy constructor
    PointT(const PointT&) = default;

    /// @brief Destructor
    ~PointT() = default;

    /// @brief Default assignment operator
    PointT& operator=( const PointT& ) = default;

    /// @brief X accessor
    /// @returns X coordinate
    float               X() const { return m_Coords.m_X; }

    /// @brief Y accessor
    /// @returns Y coordinate
    float               Y() const { return m_Coords.m_Y; }

    /// @brief Z accessor
    /// @returns Z coordinate
    float               Z() const { return m_Coords.m_Z; }

private:
    Triple              m_Coords;
};

typedef PointT<0> Point;

}
}
#endif

