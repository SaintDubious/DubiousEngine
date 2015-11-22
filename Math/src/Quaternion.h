#ifndef INCLUDED_MATH_QUATERNION
#define INCLUDED_MATH_QUATERNION

#include "Triple.h"

namespace MathTest { class QuaternionTest; }

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

template <int T> class UnitVectorT;

// When you're looking up the matrix element by column and row
// it can be helpful to have something a bit more meaningul then
// an array index
namespace MatrixIndex 
{
    // Column x Row
    static const int _11 = 0;
    static const int _21 = 1;
    static const int _31 = 2;
    static const int _41 = 3;
    static const int _12 = 4;
    static const int _22 = 5;
    static const int _32 = 6;
    static const int _42 = 7;
    static const int _13 = 8;
    static const int _23 = 9;
    static const int _33 = 10;
    static const int _43 = 11;
    static const int _14 = 12;
    static const int _24 = 13;
    static const int _34 = 14;
    static const int _44 = 15;
}

template <int T> class QuaternionT;
template <int T> bool operator==( const QuaternionT<T>& A, const QuaternionT<T>& B );
template <int T> bool operator!=( const QuaternionT<T>& A, const QuaternionT<T>& B );
template <int T> QuaternionT<T> operator*( const QuaternionT<T>& A, const QuaternionT<T>& B );

/// @brief Unit Quaternion
///
/// Everyone's favorite 4 dimensional, completely mysterious 
/// mathmatical construct. The Unit Quaternion is meant to represent
/// rotations in this game engine. There are a million quaternion
/// resources online. Here are some I'm fond of:
/// http://www.cprogramming.com/tutorial/3d/quaternions.html
/// http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/
/// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/index.htm
template <int T>
class QuaternionT
{
public:
    /// @brief Default Constructor
    ///
    /// The default constructor will create a rotation that represents
    /// Axis aligned with global X, Y, and Z. In other words this will
    /// create exactly what you'd expect for a default constructor
    QuaternionT()
        : m_Real( 1.0f )
    {}

    /// @brief Construct from axis and angle
    ///
    /// This is a more useful constructor where you can specify an
    /// axis and an angle around that axis. This is a right hand coordinate
    /// system, so the angle is counter-clockwise rotation around the
    /// axis
    /// @param Axis - [in] The axis around which we will rotate
    /// @param Angle - [in] Magnitude of Rotation (in radians)
    QuaternionT( const UnitVectorT<T>& Axis, float Angle );

    /// @brief Default copy constructor
    QuaternionT( const QuaternionT& ) = default;

    /// @brief Default destructor
    ~QuaternionT() = default;

    /// @brief Default assignment operator
    QuaternionT& operator=( const QuaternionT& ) = default;

    /// @brief Get the rotation matrix
    ///
    /// Return the rotation matrix representation of this quaternion.
    /// This is basically a shortcut for OpenGL rotation as it takes
    /// this same matrix as an input. It's also handy for finding 
    /// the three axis of rotation in one call
    /// @param Matrix - [out] Will be filled with the rotation matrix
    void GetMatrix( float Matrix[16] ) const;

private:
    friend class MathTest::QuaternionTest;
    friend bool operator== <>( const QuaternionT<T>& A, const QuaternionT<T>& B );
    friend bool operator!= <>( const QuaternionT<T>& A, const QuaternionT<T>& B );
    friend QuaternionT<T> operator* <>( const QuaternionT<T>& A, const QuaternionT<T>& B );

    void                Normalize();

    float               m_Real;
    Triple              m_Imaginary;
};

//////////////////////////////////////////////////////////////
template<int T> 
bool operator==( const QuaternionT<T>& A, const QuaternionT<T>& B )
{
    return Equals( A.m_Real, B.m_Real) &&
           A.m_Imaginary == B.m_Imaginary;
}

//////////////////////////////////////////////////////////////
template<int T> 
bool operator!=( const QuaternionT<T>& A, const QuaternionT<T>& B )
{
    return !Equals( A.m_Real, B.m_Real) ||
           A.m_Imaginary != B.m_Imaginary;
}

////////////////////////////////////////////////////////////////////////////////////
template <int T> 
QuaternionT<T> operator*( const QuaternionT<T>& A, const QuaternionT<T>& B )
{
    QuaternionT<T> Result;

    Result.m_Real          = A.m_Real*B.m_Real          - A.m_Imaginary.m_X*B.m_Imaginary.m_X - A.m_Imaginary.m_Y*B.m_Imaginary.m_Y - A.m_Imaginary.m_Z*B.m_Imaginary.m_Z;
    Result.m_Imaginary.m_X = A.m_Real*B.m_Imaginary.m_X + A.m_Imaginary.m_X*B.m_Real          + A.m_Imaginary.m_Y*B.m_Imaginary.m_Z - A.m_Imaginary.m_Z*B.m_Imaginary.m_Y;
    Result.m_Imaginary.m_Y = A.m_Real*B.m_Imaginary.m_Y - A.m_Imaginary.m_X*B.m_Imaginary.m_Z + A.m_Imaginary.m_Y*B.m_Real          + A.m_Imaginary.m_Z*B.m_Imaginary.m_X;
    Result.m_Imaginary.m_Z = A.m_Real*B.m_Imaginary.m_Z + A.m_Imaginary.m_X*B.m_Imaginary.m_Y - A.m_Imaginary.m_Y*B.m_Imaginary.m_X + A.m_Imaginary.m_Z*B.m_Real;
    // I'm not sure if we need to normalize or not after a multiplication.
    // Holding the code handy just in case
 //   Result.Normalize();

    return Result;
}

typedef QuaternionT<0> Quaternion;

}
}

#endif
