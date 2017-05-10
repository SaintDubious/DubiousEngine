#ifndef INCLUDED_MATH_POINT
#define INCLUDED_MATH_POINT

#include "Triple.h"
#include "Vector.h"
#include "Unit_vector.h"

namespace Dubious {
namespace Math {

template <int T> class PointT;
template <int T> bool operator==( const PointT<T>& a, const PointT<T>& b );
template <int T> bool operator!=( const PointT<T>& a, const PointT<T>& b );
template <int T> std::ostream& operator<<(std::ostream& o, const PointT<T>& a);

/// @brief A 3D Point
///
/// Represents a 3D point. This is different from a Vector in that
/// it is not a direction, it is simply a point in space. It is
/// a template only so that we can create Points intended for different
/// uses that the compiler can enforce
template <int T>
class PointT {
public:
    /// @brief Default Constructor
    ///
    /// Creates a Point at 0,0,0
    PointT() = default;

    /// @brief Constructor
    ///
    /// Creates the point at the coordinate specified
    /// @param X - [in] X component
    /// @param Y - [in] Y component
    /// @param Z - [in] Z component
    PointT( float x, float y, float z )
        : m_coords( x, y, z )
    {}
    
    /// @brief X accessor
    /// @returns X coordinate
    float               x() const { return m_coords.m_x; }

    /// @brief Y accessor
    /// @returns Y coordinate
    float               y() const { return m_coords.m_y; }

    /// @brief Z accessor
    /// @returns Z coordinate
    float               z() const { return m_coords.m_z; }

private:
    friend bool operator== <>( const PointT<T>& a, const PointT<T>& b );
    friend bool operator!= <>( const PointT<T>& a, const PointT<T>& b );
    friend std::ostream& operator<< <>(std::ostream& o, const PointT<T>& a);

    Triple              m_coords;
};

template <int T> 
VectorT<T> operator-( const PointT<T>& a, const PointT<T>& b )
{
    return VectorT<T>( a.x()-b.x(), a.y()-b.y(), a.z()-b.z() );
}

template <int T> 
PointT<T> operator+( const PointT<T>& a, const VectorT<T>& b )
{
    return PointT<T>( a.x()+b.x(), a.y()+b.y(), a.z()+b.z() );
}

template <int T> 
PointT<T> operator-( const PointT<T>& a, const VectorT<T>& b )
{
    return PointT<T>( a.x()-b.x(), a.y()-b.y(), a.z()-b.z() );
}

template <int T> 
PointT<T> operator+( const PointT<T>& a, const Unit_vectorT<T>& b )
{
    return PointT<T>( a.x()+b.x(), a.y()+b.y(), a.z()+b.z() );
}

template <int T> 
PointT<T> operator-( const PointT<T>& a, const Unit_vectorT<T>& b )
{
    return PointT<T>( a.x()-b.x(), a.y()-b.y(), a.z()-b.z() );
}

template<int T> 
bool operator==( const PointT<T>& a, const PointT<T>& b )
{
    return a.m_coords == b.m_coords;
}

template<int T> 
bool operator!=( const PointT<T>& a, const PointT<T>& b )
{
    return a.m_coords != b.m_coords;
}

template<int T> 
std::ostream& operator<<(std::ostream& o, const PointT<T>& a)
{
    o << a.m_coords;
    return o;
}

typedef PointT<0> Point;
typedef PointT<1> Local_point;

}
}
#endif

