#ifndef INCLUDED_MATH_QUATERNION
#define INCLUDED_MATH_QUATERNION

#include "Vector.h"
#include "Vector_math.h"

namespace Dubious {
namespace Math {

template <int T>
class QuaternionT;

/// @brief Quaternion
///
/// Everyone's favorite 4 dimensional, completely mysterious mathmatical construct.
/// While a Unit Quaternion is useful for handling rotations in 3D, the humble
/// non-Unit version appears in the middle of equations that have Unit Quaternions
/// on each end. Rather then have a Unit Quaternion that was sometimes not, I
/// decided to make a specific instance of this. It's mostly the basic math as
/// described all over the web (sometimes correctly).
///
/// Here are some resources I found useful in understanding Quaternions
/// https://www.youtube.com/watch?v=mHVwd8gYLnI
/// https://www.3dgep.com/understanding-quaternions/
template <int T>
class QuaternionT {
public:
    /// @brief Default Constructor
    QuaternionT() = default;

    /// @brief Constructor
    ///
    /// Made from constituent parts
    /// @param w - [in] the real component
    /// @param v - [in] the imaginary component
    QuaternionT(float w, const VectorT<T>& v) : m_w(w), m_v(v) {}

    /// @brief real accessor
    float w() const { return m_w; }

    /// @brief imaginary accessor
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
