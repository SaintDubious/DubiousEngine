#ifndef INCLUDED_MATH_VECTOR
#define INCLUDED_MATH_VECTOR

#include "Triple.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

template <int T> class VectorT;
template <int T> class UnitVectorT;
template <int T> VectorT<T> operator+( const VectorT<T>& A, const VectorT<T>& B );
template <int T> VectorT<T> operator*( const VectorT<T>& A, float B );

/// @brief A 3D Vector
///
/// This class represents a 3D vector. This is for non-unit vectors.
/// The template parameter is only so the compiler can enforce
/// different meanings for Vectors (ie local versus global)
template <int T>
class VectorT
{
public:
    /// @brief Default Constructor
    ///
    /// Creates a Vector of 0 length
    VectorT()
    {}

    /// @brief Constructor
    ///
    /// Creates the vector with the given X, Y, and Z
    /// @param X - [in] X component
    /// @param Y - [in] Y component
    /// @param Z - [in] Z component
    VectorT(float X, float Y, float Z)
        : m_Coords(X, Y, Z)
    {}

    /// @brief Construct from Unit Vector
    ///
    /// A simple construction from a Unit Vector. Note that
    /// the template parameter must match, so Local Vectors
    /// can be constructed from Local Unit Vectors, but not
    /// Global Unit Vectors
    /// @param U - [in] Unit Vector to copy
    VectorT( const UnitVectorT<T>& U );

    /// @brief Default copy constructor
    VectorT( const VectorT& ) = default;

    /// @brief Destructor
    ~VectorT() = default;

    /// @brief Default assignment operator
    VectorT& operator=(const VectorT&) = default;

    /// @brief Length Squared
    ///
    /// Often we don't need the actual length, but we may need to
    /// compare the relative lengths of 2 vectors. The length
    /// squared is a good way to do that as it doesn't require
    /// the costly sqrt function
    /// @returns The length of the vector squared
    float               LengthSquared() const;

    /// @brief Length
    ///
    /// The length of the vector
    /// @returns The Vector length
    float               Length() const;

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
    friend VectorT<T>   operator+ <>( const VectorT<T>& A, const VectorT<T>& B );
    friend VectorT<T>   operator* <>( const VectorT<T>& A, float B );

    VectorT( const Triple& Coords )
        : m_Coords( Coords )
    {}

    Triple              m_Coords;
};

//////////////////////////////////////////////////////////////
template <int T> 
VectorT<T> operator+( const VectorT<T>& A, const VectorT<T>& B )
{
    return VectorT<T>( A.m_Coords + B.m_Coords );
}

//////////////////////////////////////////////////////////////
template <int T> 
VectorT<T> operator*( const VectorT<T>& A, float B )
{
    return VectorT<T>( A.m_Coords.m_X*B, A.m_Coords.m_Y*B, A.m_Coords.m_Z*B );
}

typedef VectorT<0>      Vector;
typedef VectorT<1>      LocalVector;
}
}

#endif

