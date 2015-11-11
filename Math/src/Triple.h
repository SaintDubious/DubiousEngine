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
struct Triple
{
    /// @brief Default Constructor
    ///
    /// Initializes a Triple to 0,0,0
    Triple()
        : m_X( 0 )
        , m_Y( 0 )
        , m_Z( 0 )
    {}

    /// @brief Constructor
    ///
    /// Creates a triple with the passed in values
    /// @param X - [in] X component
    /// @param Y - [in] Y component
    /// @param Z - [in] Z component
    Triple(float X, float Y, float Z)
        : m_X(X)
        , m_Y(Y)
        , m_Z(Z)
    {}

    /// @brief Default copy constructor
    Triple(const Triple&) = default;

    /// @brief Destructor
    ~Triple() = default;

    /// @brief Default assignment operator
    Triple& operator=(const Triple&) = default;

    float               m_X;
    float               m_Y;
    float               m_Z;
};

//////////////////////////////////////////////////////////////
inline bool operator==( const Triple& A, const Triple& B )
{
    return Equals(A.m_X, B.m_X) &&
           Equals(A.m_Y, B.m_Y) &&
           Equals(A.m_Z, B.m_Z);
}

//////////////////////////////////////////////////////////////
inline bool operator!=(const Triple& A, const Triple& B)
{
    return !Equals(A.m_X, B.m_X) ||
           !Equals(A.m_Y, B.m_Y) ||
           !Equals(A.m_Z, B.m_Z);
}

//////////////////////////////////////////////////////////////
inline Triple operator+(const Triple& A, const Triple& B)
{
    return Triple(A.m_X + B.m_X, A.m_Y + B.m_Y, A.m_Z + B.m_Z);
}

//////////////////////////////////////////////////////////////
inline Triple operator-(const Triple& A, const Triple& B)
{
    return Triple(A.m_X - B.m_X, A.m_Y - B.m_Y, A.m_Z - B.m_Z);
}

//////////////////////////////////////////////////////////////
inline std::ostream& operator<<(std::ostream& o, const Triple& A)
{
    o << "(" << A.m_X << ", " << A.m_Y << ", " << A.m_Z << ")";
    return o;
}

}
}

#endif