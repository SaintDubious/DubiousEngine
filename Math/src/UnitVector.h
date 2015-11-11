#ifndef INCLUDED_MATH_UNITVECTOR
#define INCLUDED_MATH_UNITVECTOR

#include "Triple.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

template <int T> class VectorT;

template <int T> class UnitVectorT;
template <int T> bool operator==( const UnitVectorT<T>& A, const UnitVectorT<T>& B );
template <int T> bool operator!=( const UnitVectorT<T>& A, const UnitVectorT<T>& B );

/// @brief A 3D Unit Vector
///
/// This class represents a 3D Unit Vector.
/// The template parameter is only so the compiler can enforce
/// different meanings for Unit Vectors (ie local versus global)
template <int T>
class UnitVectorT
{
public:
    /// @brief Constructor
    ///
    /// Creates the unit vector with the given X, Y, and Z converted
    /// to a length of 1
    /// @param X - [in] X component
    /// @param Y - [in] Y component
    /// @param Z - [in] Z component
    UnitVectorT(float X, float Y, float Z);

    /// @brief Constructor
    ///
    /// Creates a Unit Vector from the passed in Vector. This
    /// serves as a Normalize function
    /// @param V - [in] Vector to copy
    UnitVectorT(const VectorT<T>& V);

    /// @brief Default copy constructor
    UnitVectorT(const UnitVectorT&) = default;

    /// @brief Destructor
    ~UnitVectorT() = default;

    /// @brief Default assignment operator
    UnitVectorT& operator=(const UnitVectorT&) = default;

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
    friend bool operator== <>( const UnitVectorT<T>& A, const UnitVectorT<T>& B );
    friend bool operator!= <>( const UnitVectorT<T>& A, const UnitVectorT<T>& B );

    Triple              m_Coords;
};

//////////////////////////////////////////////////////////////
template<int T> 
bool operator==( const UnitVectorT<T>& A, const UnitVectorT<T>& B )
{
    return A.m_Coords == B.m_Coords;
}

//////////////////////////////////////////////////////////////
template<int T> 
bool operator!=( const UnitVectorT<T>& A, const UnitVectorT<T>& B )
{
    return A.m_Coords != B.m_Coords;
}

typedef UnitVectorT<0>  UnitVector;
}
}

#endif

