#ifndef INCLUDED_MATH_QUATERNION
#define INCLUDED_MATH_QUATERNION

#include "Triple.h"

namespace Math_test { class Quaternion_test; }

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

template <int T> class Unit_vectorT;

// When you're looking up the matrix element by column and row
// it can be helpful to have something a bit more meaningful then
// an array index
namespace Matrix_index 
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
template <int T> bool operator==( const QuaternionT<T>& a, const QuaternionT<T>& b );
template <int T> bool operator!=( const QuaternionT<T>& a, const QuaternionT<T>& b );
template <int T> QuaternionT<T> operator*( const QuaternionT<T>& a, const QuaternionT<T>& b );

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
class QuaternionT {
public:
    /// @brief Default Constructor
    ///
    /// The default constructor will create a rotation that represents
    /// Axis aligned with global X, Y, and Z. In other words this will
    /// create exactly what you'd expect for a default constructor
    QuaternionT() = default;

    /// @brief Construct from axis and angle
    ///
    /// This is a more useful constructor where you can specify an
    /// axis and an angle around that axis. This is a right hand coordinate
    /// system, so the angle is counter-clockwise rotation around the
    /// axis
    /// @param axis - [in] The axis around which we will rotate
    /// @param angle - [in] Magnitude of Rotation (in radians)
    QuaternionT( const Unit_vectorT<T>& axis, float angle );

    /// @brief The conjugate
    ///
    /// This is needed to rotate a Vector by the rotation represented
    /// by this quaternion. The conjugate is simply:
    /// (w, -x, -y, -z)
    /// @returns the conjugate
    QuaternionT         conjugate() const;

    /// @brief Get the rotation matrix
    ///
    /// Return the rotation matrix representation of this quaternion.
    /// This is basically a shortcut for OpenGL rotation as it takes
    /// this same matrix as an input. It's also handy for finding 
    /// the three axis of rotation in one call
    /// @param matrix - [out] Will be filled with the rotation matrix
    void                get_matrix( float matrix[16] ) const;

private:
    friend class Math_test::Quaternion_test;
    friend class Coordinate_space;
    friend bool operator== <>( const QuaternionT<T>& a, const QuaternionT<T>& b );
    friend bool operator!= <>( const QuaternionT<T>& a, const QuaternionT<T>& b );
    friend QuaternionT<T> operator* <>( const QuaternionT<T>& a, const QuaternionT<T>& b );

    QuaternionT( float real, const Triple& imaginary )
        : m_real( real )
        , m_imaginary( imaginary )
    {}

    void                normalize();

    float               m_real = 1.0f;
    Triple              m_imaginary;
};

//////////////////////////////////////////////////////////////
template<int T> 
bool operator==( const QuaternionT<T>& a, const QuaternionT<T>& b )
{
    return equals( a.m_real, b.m_real) &&
           a.m_imaginary == b.m_imaginary;
}

//////////////////////////////////////////////////////////////
template<int T> 
bool operator!=( const QuaternionT<T>& a, const QuaternionT<T>& b )
{
    return !equals( a.m_real, b.m_real) ||
           a.m_imaginary != b.m_imaginary;
}

//////////////////////////////////////////////////////////////
template <int T> 
QuaternionT<T> operator*( const QuaternionT<T>& a, const QuaternionT<T>& b )
{
    QuaternionT<T> result;

    result.m_real          = a.m_real*b.m_real          - a.m_imaginary.m_x*b.m_imaginary.m_x - a.m_imaginary.m_y*b.m_imaginary.m_y - a.m_imaginary.m_z*b.m_imaginary.m_z;
    result.m_imaginary.m_x = a.m_real*b.m_imaginary.m_x + a.m_imaginary.m_x*b.m_real          + a.m_imaginary.m_y*b.m_imaginary.m_z - a.m_imaginary.m_z*b.m_imaginary.m_y;
    result.m_imaginary.m_y = a.m_real*b.m_imaginary.m_y - a.m_imaginary.m_x*b.m_imaginary.m_z + a.m_imaginary.m_y*b.m_real          + a.m_imaginary.m_z*b.m_imaginary.m_x;
    result.m_imaginary.m_z = a.m_real*b.m_imaginary.m_z + a.m_imaginary.m_x*b.m_imaginary.m_y - a.m_imaginary.m_y*b.m_imaginary.m_x + a.m_imaginary.m_z*b.m_real;
    // I'm not sure if we need to normalize or not after a multiplication.
    // Holding the code handy just in case
 //   result.normalize();

    return result;
}

typedef QuaternionT<0> Quaternion;
typedef QuaternionT<1> Local_quaternion;

}
}

#endif
