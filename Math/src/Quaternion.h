#ifndef INCLUDED_MATH_QUATERNION
#define INCLUDED_MATH_QUATERNION

#include "Vector.h"
#include "Vector_math.h"

namespace Dubious {
namespace Math {

template <int T>
class QuaternionT;

/// @brief Unit Quaternion
///
/// https://www.youtube.com/watch?v=mHVwd8gYLnI
/// https://www.3dgep.com/understanding-quaternions/

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

    QuaternionT(float w, const VectorT<T>& v) : m_w(w), m_v(v) {}

    float w() const { return m_w; }

    const VectorT<T>& v() const { return m_v; }

private:
    float      m_w = 0.0f;
    VectorT<T> m_v;
};

template <int T>
bool
operator==(const QuaternionT<T>& a, const QuaternionT<T>& b)
{
    return equals(a.w(), b.w()) && a.v() == b.v();
}

template <int T>
bool
operator!=(const QuaternionT<T>& a, const QuaternionT<T>& b)
{
    return !(a == b);
}

template <int T>
QuaternionT<T>
operator+(const QuaternionT<T>& a, const QuaternionT<T>& b)
{
    return QuaternionT<T>(a.w() + b.w(), a.v() + b.v());
}

template <int T>
QuaternionT<T>
operator-(const QuaternionT<T>& a, const QuaternionT<T>& b)
{
    return QuaternionT<T>(a.w() - b.w(), a.v() - b.v());
}

template <int T>
QuaternionT<T> operator*(const QuaternionT<T>& a, const QuaternionT<T>& b)
{
    return QuaternionT<T>(a.w() * b.w() - dot_product(a.v(), b.v()),
                          (a.w() * b.v()) + (b.w() * a.v()) + (cross_product(a.v(), b.v())));
}

template <int T>
QuaternionT<T> operator*(const VectorT<T>& a, const QuaternionT<T>& b)
{
    return QuaternionT<T>(0, a) * b;
}

template <int T>
QuaternionT<T> operator*(const QuaternionT<T>& a, const VectorT<T>& b)
{
    return a * QuaternionT<T>(0, b);
}

template <int T>
QuaternionT<T> operator*(const QuaternionT<T>& a, float b)
{
    return QuaternionT<T>(a.w() * b, a.v() * b);
}

template <int T>
QuaternionT<T> operator*(float a, const QuaternionT<T>& b)
{
    return b * a;
}

template <int T>
std::ostream&
operator<<(std::ostream& o, const QuaternionT<T>& q)
{
    o << "(" << q.w() << ", " << q.v() << ")";
    return o;
}

typedef QuaternionT<0> Quaternion;
typedef QuaternionT<1> Local_quaternion;

}  // namespace Math
}  // namespace Dubious

#endif
