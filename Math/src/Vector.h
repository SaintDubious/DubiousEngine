#ifndef INCLUDED_MATH_VECTOR
#define INCLUDED_MATH_VECTOR

#include "Triple.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

template <int T> class VectorT;
template <int T> class Unit_vectorT;
template <int T> bool operator==( const VectorT<T>& a, const VectorT<T>& b );
template <int T> bool operator!=( const VectorT<T>& a, const VectorT<T>& b );
template <int T> VectorT<T> operator+( const VectorT<T>& a, const VectorT<T>& b );
template <int T> VectorT<T> operator-( const VectorT<T>& a, const VectorT<T>& b );
template <int T> VectorT<T> operator*( const VectorT<T>& a, float b );
template <int T> VectorT<T> operator*( float a, const VectorT<T>& b );
template <int T> VectorT<T> operator/( const VectorT<T>& a, float b );
template <int T> std::ostream& operator<<(std::ostream& o, const VectorT<T>& a);

/// @brief A 3D Vector
///
/// This class represents a 3D vector. This is for non-unit vectors.
/// The template parameter is only so the compiler can enforce
/// different meanings for Vectors (ie local versus global)
template <int T>
class VectorT {
public:
    /// @brief Default Constructor
    ///
    /// Creates a Vector of 0 length
    VectorT() = default;

    /// @brief Constructor
    ///
    /// Creates the vector with the given X, Y, and Z
    /// @param x - [in] X component
    /// @param y - [in] Y component
    /// @param z - [in] Z component
    VectorT(float x, float y, float z)
        : m_coords(x, y, z)
    {}

    /// @brief Construct from Unit Vector
    ///
    /// A simple construction from a Unit Vector. Note that
    /// the template parameter must match, so Local Vectors
    /// can be constructed from Local Unit Vectors, but not
    /// Global Unit Vectors
    /// @param U - [in] Unit Vector to copy
    VectorT( const Unit_vectorT<T>& u );

    /// @brief Length Squared
    ///
    /// Often we don't need the actual length, but we may need to
    /// compare the relative lengths of 2 vectors. The length
    /// squared is a good way to do that as it doesn't require
    /// the costly sqrt function
    /// @returns The length of the vector squared
    float               length_squared() const;

    /// @brief Length
    ///
    /// The length of the vector
    /// @returns The Vector length
    float               length() const;

    /// @brief X accessor
    /// @returns X coordinate
    float               x() const { return m_coords.m_x; }

    /// @brief Y accessor
    /// @returns Y coordinate
    float               y() const { return m_coords.m_y; }

    /// @brief Z accessor
    /// @returns Z coordinate
    float               z() const { return m_coords.m_z; }

    friend bool         operator== <>( const VectorT<T>& a, const VectorT<T>& b );
    friend bool         operator!= <>( const VectorT<T>& a, const VectorT<T>& b );
    friend VectorT<T>   operator+ <>( const VectorT<T>& a, const VectorT<T>& b );
    friend VectorT<T>   operator- <>( const VectorT<T>& a, const VectorT<T>& b );
    friend VectorT<T>   operator* <>( const VectorT<T>& a, float b );
    friend VectorT<T>   operator* <>( float a, const VectorT<T>& b );
    friend VectorT<T>   operator/ <>( const VectorT<T>& a, float b );
    friend std::ostream& operator<< <>(std::ostream& o, const VectorT<T>& a);

    VectorT<T>          operator-() const { return VectorT<T>( -m_coords ); }
    VectorT<T>&         operator+=( const VectorT<T>& rhs );
    VectorT<T>&         operator-=( const VectorT<T>& rhs );

private:
    VectorT( const Triple& coords )
        : m_coords( coords )
    {}

    Triple              m_coords;
};

//////////////////////////////////////////////////////////////
template<int T> 
bool operator==( const VectorT<T>& a, const VectorT<T>& b )
{
    return a.m_coords == b.m_coords;
}

//////////////////////////////////////////////////////////////
template<int T> 
bool operator!=( const VectorT<T>& a, const VectorT<T>& b )
{
    return a.m_coords != b.m_coords;
}

//////////////////////////////////////////////////////////////
template <int T> 
VectorT<T> operator+( const VectorT<T>& a, const VectorT<T>& b )
{
    return VectorT<T>( a.m_coords + b.m_coords );
}

//////////////////////////////////////////////////////////////
template <int T> 
VectorT<T> operator-( const VectorT<T>& a, const VectorT<T>& b )
{
    return VectorT<T>( a.m_coords - b.m_coords );
}

//////////////////////////////////////////////////////////////
template <int T> 
VectorT<T> operator*( const VectorT<T>& a, float b )
{
    return VectorT<T>( a.m_coords.m_x*b, a.m_coords.m_y*b, a.m_coords.m_z*b );
}

//////////////////////////////////////////////////////////////
template <int T> 
VectorT<T> operator*( float a, const VectorT<T>& b )
{
    return VectorT<T>( a*b.m_coords.m_x, a*b.m_coords.m_y, a*b.m_coords.m_z );
}

//////////////////////////////////////////////////////////////
template <int T> 
VectorT<T> operator/( const VectorT<T>& a, float b )
{
    return VectorT<T>( a.m_coords.m_x/b, a.m_coords.m_y/b, a.m_coords.m_z/b );
}

//////////////////////////////////////////////////////////////
template <int T> 
std::ostream& operator<<(std::ostream& o, const VectorT<T>& a)
{
    o << a.m_coords;
    return o;
}

typedef VectorT<0>      Vector;
typedef VectorT<1>      Local_vector;
}
}

#endif

