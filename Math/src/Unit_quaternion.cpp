#include "Unit_quaternion.h"

#include "Quaternion.h"
#include "Unit_vector.h"

namespace Dubious {
namespace Math {

template <int T>
Unit_quaternionT<T>::Unit_quaternionT(const QuaternionT<T>& q) : m_w(q.w()), m_v(q.v())
{
    normalize();
}

template <int T>
Unit_quaternionT<T>::Unit_quaternionT(const Unit_vectorT<T>& axis, float angle)
    : m_w(cosf(angle / 2.0f)), m_v(VectorT<T>(axis) * sinf(angle / 2.0f))
{
}

template <int T>
void
Unit_quaternionT<T>::get_matrix(float matrix[16]) const
{
    float xx = m_v.x() * m_v.x();
    float yy = m_v.y() * m_v.y();
    float zz = m_v.z() * m_v.z();

    using namespace Matrix_index;

    matrix[_11] = 1.0f - 2.0f * (yy + zz);
    matrix[_21] = 2.0f * (m_v.x() * m_v.y() + m_w * m_v.z());
    matrix[_31] = 2.0f * (m_v.x() * m_v.z() - m_w * m_v.y());
    matrix[_41] = 0.0;

    matrix[_12] = 2.0f * (m_v.x() * m_v.y() - m_w * m_v.z());
    matrix[_22] = 1.0f - 2.0f * (xx + zz);
    matrix[_32] = 2.0f * (m_v.y() * m_v.z() + m_w * m_v.x());
    matrix[_42] = 0.0;

    matrix[_13] = 2.0f * (m_v.x() * m_v.z() + m_w * m_v.y());
    matrix[_23] = 2.0f * (m_v.y() * m_v.z() - m_w * m_v.x());
    matrix[_33] = 1.0f - 2.0f * (xx + yy);
    matrix[_43] = 0.0;

    matrix[_14] = 0.0;
    matrix[_24] = 0.0;
    matrix[_34] = 0.0;
    matrix[_44] = 1.0;
}

template <int T>
std::tuple<Unit_vectorT<T>, float>
Unit_quaternionT<T>::get_axis_angle() const
{
    // The length of the m_v vector is not correct, however
    // in our case it will be normalized anyway, so the length
    // isn't important.
    return std::make_tuple(Unit_vectorT<T>(m_v), 2 * acosf(m_w));
}

template <int T>
float
Unit_quaternionT<T>::magnitude() const
{
    return sqrt((m_w * m_w) + m_v.length_squared());
}

template <int T>
void
Unit_quaternionT<T>::normalize()
{
    float mag = magnitude();
    if (equals(mag, 0)) {
        throw std::runtime_error("Quaternion has 0 magnitude");
    }
    m_w = m_w / mag;
    m_v = m_v / mag;
}

template class Unit_quaternionT<0>;
template class Unit_quaternionT<1>;

}  // namespace Math
}  // namespace Dubious
