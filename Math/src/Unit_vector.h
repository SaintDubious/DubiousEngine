#ifndef INCLUDED_MATH_UNITVECTOR
#define INCLUDED_MATH_UNITVECTOR

#include "Triple.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

template <int T> class VectorT;

template <int T> class Unit_vectorT;
template <int T> bool operator==( const Unit_vectorT<T>& a, const Unit_vectorT<T>& b );
template <int T> bool operator!=( const Unit_vectorT<T>& a, const Unit_vectorT<T>& b );

/// @brief A 3D Unit Vector
///
/// This class represents a 3D Unit Vector.
/// The template parameter is only so the compiler can enforce
/// different meanings for Unit Vectors (ie local versus global)
template <int T>
class Unit_vectorT {
public:
    /// @brief Constructor
    ///
    /// This is primarily an optimization. In some cases you need to
    /// create an empty UnitVector so that you can assign it or
    /// pass it as an out parameter to something. In this case you
    /// don't really care what value it holds. Creating one with
    /// dummy args causes a painful normalization. So this function
    /// will create one as 1,0,0 and not call normalization
    Unit_vectorT() = default;

    /// @brief Constructor
    ///
    /// Creates the unit vector with the given X, Y, and Z converted
    /// to a length of 1
    /// @param x - [in] X component
    /// @param y - [in] Y component
    /// @param z - [in] Z component
    Unit_vectorT(float x, float y, float z);

    /// @brief Constructor
    ///
    /// Creates a Unit Vector from the passed in Vector. This
    /// serves as a Normalize function
    /// @param V - [in] Vector to copy
    Unit_vectorT(const VectorT<T>& v);

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
    friend bool operator== <>( const Unit_vectorT<T>& a, const Unit_vectorT<T>& b );
    friend bool operator!= <>( const Unit_vectorT<T>& a, const Unit_vectorT<T>& b );

    Triple              m_coords = {1, 0, 0};
};

//////////////////////////////////////////////////////////////
template<int T> 
bool operator==( const Unit_vectorT<T>& a, const Unit_vectorT<T>& b )
{
    return a.m_coords == b.m_coords;
}

//////////////////////////////////////////////////////////////
template<int T> 
bool operator!=( const Unit_vectorT<T>& a, const Unit_vectorT<T>& b )
{
    return a.m_coords != b.m_coords;
}

typedef Unit_vectorT<0>  Unit_vector;
typedef Unit_vectorT<1>  Local_unit_vector;
}
}

#endif
