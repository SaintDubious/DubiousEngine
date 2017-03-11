#ifndef INCLUDED_MATH_QUATERNION
#define INCLUDED_MATH_QUATERNION

#include "Triple.h"
#include "Vector.h"

#include <tuple>

namespace Math_test { class Quaternion_test; }
namespace Physics_test { class Collision_solver_test; }

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
template <int T> QuaternionT<T> operator+( const QuaternionT<T>& a, const QuaternionT<T>& b );
template <int T> QuaternionT<T> operator*( const QuaternionT<T>& a, const QuaternionT<T>& b );
template <int T> QuaternionT<T> operator*( const VectorT<T>& a, const QuaternionT<T>& b );
template <int T> QuaternionT<T> operator*( const QuaternionT<T>& a, float b );
template <int T> std::ostream& operator<<(std::ostream& o, const QuaternionT<T>& q);

/// @brief Unit Quaternion
///
/// Everyone's favorite 4 dimensional, completely mysterious 
/// mathmatical construct. The Unit Quaternion is meant to represent
/// rotations in this game engine. There are a million quaternion
/// resources online. Here are some I'm fond of:
/// http://www.cprogramming.com/tutorial/3d/quaternions.html
/// http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/
/// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/index.htm
///
/// I'm not sure exactly when to make sure it's a Unit Quaternion and when it can
/// be just a regular one. Operations like scalar multiply will result in a
/// non-Unit Quaternion. I gather from docs that only a Unit Quaternion represents
/// a rotation. The Physics Integrator has steps where non-Unit Quaternions emerge
/// and are used to create rotations. Maybe they have to then be normalized in the
/// end? Maybe this class should have a plain and unit version like my Vectors?
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

    /// @brief Get the Axis and Angle
    ///
    /// Return a unit vector representing the axis of rotation, as
    /// well as a float representing the angle around that axis, in
    /// radians
    /// @returns tuple of axis and angle
    std::tuple<Unit_vectorT<T>,float> get_axis_angle() const;

    /// @brief Normalize the Quaternion
    ///
    /// As per the class comment, I'm not entirely sure when this needs
    /// to be Unit and when it can be a plain Quaternion. This method
    /// will make it a Unit Quaternion
    void                normalize();

    /// @brief Quaternion magnitude (like length)
    ///
    /// @returns magnitude
    float               magnitude() const;

    /// @brief Quaternion magnitude squared
    ///
    /// Same as with the Vecrot.length_squared. Sometimes you want
    /// a comparable magnitude without the cost of a sqrt
    /// @returns magnitude squared
    float               magnitude_squared() const;

private:
    friend class Math_test::Quaternion_test;
    friend class Physics_test::Collision_solver_test;
    friend class Coordinate_space;
    friend bool operator== <>( const QuaternionT<T>& a, const QuaternionT<T>& b );
    friend bool operator!= <>( const QuaternionT<T>& a, const QuaternionT<T>& b );
    friend QuaternionT<T> operator+ <>( const QuaternionT<T>& a, const QuaternionT<T>& b );
    friend QuaternionT<T> operator* <>( const QuaternionT<T>& a, const QuaternionT<T>& b );
    friend QuaternionT<T> operator* <>( const VectorT<T>& a, const QuaternionT<T>& b );
    friend QuaternionT<T> operator* <>( const QuaternionT<T>& a, float b );
    friend std::ostream& operator<< <>(std::ostream& o, const QuaternionT<T>& q);

    QuaternionT( float real, const Triple& imaginary )
        : m_real( real )
        , m_imaginary( imaginary )
    {}

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
QuaternionT<T> operator+( const QuaternionT<T>& a, const QuaternionT<T>& b )
{
    QuaternionT<T> result;

    result.m_real       = a.m_real+b.m_real;
    result.m_imaginary  = a.m_imaginary+b.m_imaginary;

    return result;
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

    return result;
}

//////////////////////////////////////////////////////////////
template <int T> 
QuaternionT<T> operator*( const VectorT<T>& a, const QuaternionT<T>& b )
{ 
    return Quaternion( 0, Triple( a.x(), a.y(), a.z() ) ) * b;
}

//////////////////////////////////////////////////////////////
template <int T> 
QuaternionT<T> operator*( const QuaternionT<T>& a, float b )
{
    QuaternionT<T> result;

    result.m_real           = a.m_real * b;
    result.m_imaginary.m_x  = a.m_imaginary.m_x * b;
    result.m_imaginary.m_y  = a.m_imaginary.m_y * b;
    result.m_imaginary.m_z  = a.m_imaginary.m_z * b;

    return result;
}

//////////////////////////////////////////////////////////////
template <int T> 
std::ostream& operator<<(std::ostream& o, const QuaternionT<T>& q)
{
    o << "(" << q.m_real << ", " << q.m_imaginary << ")";
    return o;
}

typedef QuaternionT<0> Quaternion;
typedef QuaternionT<1> Local_quaternion;

}
}

#endif
