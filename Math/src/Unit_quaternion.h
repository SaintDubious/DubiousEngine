#ifndef INCLUDED_MATH_UNITQUATERNION
#define INCLUDED_MATH_UNITQUATERNION

#include "Vector.h"
#include "Vector_math.h"

#include <tuple>

namespace Math_test {
class Unit_quaternion_test;
}
namespace Physics_test {
class Collision_solver_test;
}

namespace Dubious {
namespace Math {

// When you're looking up the matrix element by column and row
// it can be helpful to have something a bit more meaningful then
// an array index
namespace Matrix_index {
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
}  // namespace Matrix_index

template <int T>
class Unit_vectorT;
template <int T>
class QuaternionT;
template <int T>
class Unit_quaternionT;
template <int T>
Unit_quaternionT<T> operator*(const Unit_quaternionT<T>& a, const Unit_quaternionT<T>& b);

template <int T>
class Unit_quaternionT {
public:
    Unit_quaternionT() = default;

    Unit_quaternionT(const QuaternionT<T>& q);

    /// @brief Construct from axis and angle
    ///
    /// This is a more useful constructor where you can specify an
    /// axis and an angle around that axis. This is a right hand coordinate
    /// system, so the angle is counter-clockwise rotation around the
    /// axis
    /// @param axis - [in] The axis around which we will rotate
    /// @param angle - [in] Magnitude of Rotation (in radians)
    Unit_quaternionT(const Unit_vectorT<T>& axis, float angle);

    float w() const { return m_w; }

    const VectorT<T>& v() const { return m_v; }

    /// @brief The conjugate
    ///
    /// This is needed to rotate a Vector by the rotation represented
    /// by this quaternion. The conjugate is simply:
    /// (w, -v)
    /// @returns the conjugate
    Unit_quaternionT conjugate() const { return Unit_quaternionT(m_w, -m_v); }

    /// @brief Get the rotation matrix
    ///
    /// Return the rotation matrix representation of this quaternion.
    /// This is basically a shortcut for OpenGL rotation as it takes
    /// this same matrix as an input. It's also handy for finding
    /// the three axis of rotation in one call
    /// @param matrix - [out] Will be filled with the rotation matrix
    void get_matrix(float matrix[16]) const;

    /// @brief Get the Axis and Angle
    ///
    /// Return a unit vector representing the axis of rotation, as
    /// well as a float representing the angle around that axis, in
    /// radians
    /// @returns tuple of axis and angle
    std::tuple<Unit_vectorT<T>, float> get_axis_angle() const;

    Unit_quaternionT<T> operator-() const { return Unit_quaternionT<T>(-m_w, -m_v); }

private:
    // There are cases (multiplication) where we know that the result is a
    // Unit Quaternion. We don't want to pay the cost of normalizing when
    // we know it's already normalized
    Unit_quaternionT(float w, const VectorT<T>& v) : m_w(w), m_v(v) {}

    // In a world without rounding errors the magnitude would always be 1 and
    // there would be no reason to normalize it. In the real world we may need these
    void  normalize();
    float magnitude() const;

    friend class Math_test::Unit_quaternion_test;
    friend class Physics_test::Collision_solver_test;
    friend class Coordinate_space;
    friend Unit_quaternionT<T> operator*<>(const Unit_quaternionT<T>& a,
                                           const Unit_quaternionT<T>& b);

    float      m_w = 1.0f;
    VectorT<T> m_v;
};

template <int T>
bool
operator==(const Unit_quaternionT<T>& a, const Unit_quaternionT<T>& b)
{
    return equals(a.w(), b.w()) && a.v() == b.v();
}

template <int T>
bool
operator!=(const Unit_quaternionT<T>& a, const Unit_quaternionT<T>& b)
{
    return !(a == b);
}

template <int T>
QuaternionT<T>
operator+(const Unit_quaternionT<T>& a, const Unit_quaternionT<T>& b)
{
    return QuaternionT<T>(a.w() + b.w(), a.v() + b.v());
}

template <int T>
QuaternionT<T>
operator+(const QuaternionT<T>& a, const Unit_quaternionT<T>& b)
{
    return QuaternionT<T>(a.w() + b.w(), a.v() + b.v());
}

template <int T>
QuaternionT<T>
operator+(const Unit_quaternionT<T>& a, const QuaternionT<T>& b)
{
    return QuaternionT<T>(a.w() + b.w(), a.v() + b.v());
}

template <int T>
QuaternionT<T>
operator-(const Unit_quaternionT<T>& a, const Unit_quaternionT<T>& b)
{
    return QuaternionT<T>(a.w() - b.w(), a.v() - b.v());
}

template <int T>
QuaternionT<T>
operator-(const QuaternionT<T>& a, const Unit_quaternionT<T>& b)
{
    return QuaternionT<T>(a.w() - b.w(), a.v() - b.v());
}

template <int T>
QuaternionT<T>
operator-(const Unit_quaternionT<T>& a, const QuaternionT<T>& b)
{
    return QuaternionT<T>(a.w() - b.w(), a.v() - b.v());
}

template <int T>
Unit_quaternionT<T> operator*(const Unit_quaternionT<T>& a, const Unit_quaternionT<T>& b)
{
    return Unit_quaternionT<T>(a.w() * b.w() - dot_product(a.v(), b.v()),
                               (a.w() * b.v()) + (b.w() * a.v()) + (cross_product(a.v(), b.v())));
}

template <int T>
QuaternionT<T> operator*(const Unit_quaternionT<T>& a, const QuaternionT<T>& b)
{
    return QuaternionT<T>(a.w(), a.v()) * b;
}

template <int T>
QuaternionT<T> operator*(const QuaternionT<T>& a, const Unit_quaternionT<T>& b)
{
    return a * QuaternionT<T>(b.w(), b.v());
}

template <int T>
QuaternionT<T> operator*(const Unit_quaternionT<T>& a, float b)
{
    return QuaternionT<T>(a.w() * b, a.v() * b);
}

template <int T>
QuaternionT<T> operator*(float a, const Unit_quaternionT<T>& b)
{
    return b * a;
}

template <int T>
std::ostream&
operator<<(std::ostream& o, const Unit_quaternionT<T>& q)
{
    o << "(" << q.w() << ", " << q.v() << ")";
    return o;
}

typedef Unit_quaternionT<0> Unit_quaternion;
typedef Unit_quaternionT<1> Local_unit_quaternion;

}  // namespace Math
}  // namespace Dubious

#endif
