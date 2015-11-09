#ifndef INCLUDED_MATH_UNITVECTOR
#define INCLUDED_MATH_UNITVECTOR

#include "Triple.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

/// @brief A 3D Unit Vector
///
/// This class represents a 3D Unit Vector.
/// The template parameter is only so the compiler can enforce
/// different meanings for Unit Vectors (ie local versus global)
template <int>
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

    /// @brief Destructor
    ~UnitVectorT()
    {}

    /// @brief Default copy constructor
    UnitVectorT(const UnitVectorT&) = default;

    /// @brief Default equals operator
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
    Triple              m_Coords;
};

typedef UnitVectorT<0>  UnitVector;
}
}

#endif

