#ifndef INCLUDED_MATH_POINT
#define INCLUDED_MATH_POINT

#include "Triple.h"
#include "Vector.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

template <int T> class PointT;
template <int T> bool operator==( const PointT<T>& A, const PointT<T>& B );
template <int T> bool operator!=( const PointT<T>& A, const PointT<T>& B );

/// @brief A 3D Point
///
/// Represents a 3D point. This is different from a Vector in that
/// it is not a direction, it is simply a point in space. It is
/// a template only so that we can create Points intended for different
/// uses that the compiler can enforce
template <int T>
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
    friend bool operator== <>( const PointT<T>& A, const PointT<T>& B );
    friend bool operator!= <>( const PointT<T>& A, const PointT<T>& B );

    Triple              m_Coords;
};

//////////////////////////////////////////////////////////////
template <int T> 
VectorT<T> operator-( const PointT<T>& A, const PointT<T>& B )
{
    return VectorT<T>( A.X()-B.X(), A.Y()-B.Y(), A.Z()-B.Z() );
}

//////////////////////////////////////////////////////////////
template<int T> 
bool operator==( const PointT<T>& A, const PointT<T>& B )
{
    return A.m_Coords == B.m_Coords;
}

//////////////////////////////////////////////////////////////
template<int T> 
bool operator!=( const PointT<T>& A, const PointT<T>& B )
{
    return A.m_Coords != B.m_Coords;
}

typedef PointT<0> Point;
typedef PointT<1> LocalPoint;

}
}
#endif

