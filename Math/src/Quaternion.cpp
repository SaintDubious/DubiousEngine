#include "Quaternion.h"
#include "Unit_vector.h"

namespace Dubious {
namespace Math {

template <int T>
QuaternionT<T>::QuaternionT(const Unit_vectorT<T>& axis, float angle)
{
    float half_sin = sinf(angle / 2.0f);
    m_real         = cosf(angle / 2.0f);
    m_imaginary    = Triple(axis.x() * half_sin, axis.y() * half_sin, axis.z() * half_sin);
}

template <int T>
void
QuaternionT<T>::get_matrix(float matrix[16]) const
{
    float xx(m_imaginary.m_x * m_imaginary.m_x);
    float yy(m_imaginary.m_y * m_imaginary.m_y);
    float zz(m_imaginary.m_z * m_imaginary.m_z);

    using namespace Matrix_index;

    matrix[_11] = 1.0f - 2.0f * (yy + zz);
    matrix[_21] = 2.0f * (m_imaginary.m_x * m_imaginary.m_y + m_real * m_imaginary.m_z);
    matrix[_31] = 2.0f * (m_imaginary.m_x * m_imaginary.m_z - m_real * m_imaginary.m_y);
    matrix[_41] = 0.0;

    matrix[_12] = 2.0f * (m_imaginary.m_x * m_imaginary.m_y - m_real * m_imaginary.m_z);
    matrix[_22] = 1.0f - 2.0f * (xx + zz);
    matrix[_32] = 2.0f * (m_imaginary.m_y * m_imaginary.m_z + m_real * m_imaginary.m_x);
    matrix[_42] = 0.0;

    matrix[_13] = 2.0f * (m_imaginary.m_x * m_imaginary.m_z + m_real * m_imaginary.m_y);
    matrix[_23] = 2.0f * (m_imaginary.m_y * m_imaginary.m_z - m_real * m_imaginary.m_x);
    matrix[_33] = 1.0f - 2.0f * (xx + yy);
    matrix[_43] = 0.0;

    matrix[_14] = 0.0;
    matrix[_24] = 0.0;
    matrix[_34] = 0.0;
    matrix[_44] = 1.0;
}

template <int T>
std::tuple<Unit_vectorT<T>, float>
QuaternionT<T>::get_axis_angle() const
{
    float           angle = 2 * acosf(m_real);
    float           denom = sqrtf(1 - m_real * m_real);
    Unit_vectorT<T> axis(m_imaginary.m_x / denom, m_imaginary.m_y / denom, m_imaginary.m_z / denom);
    return std::make_tuple(axis, angle);
}

template <int T>
QuaternionT<T>
QuaternionT<T>::conjugate() const
{
    return QuaternionT<T>(m_real, Triple(-m_imaginary.m_x, -m_imaginary.m_y, -m_imaginary.m_z));
}

template <int T>
void
QuaternionT<T>::normalize()
{
    float mag = magnitude();
    if (equals(mag, 0)) {
        throw std::runtime_error("Quaternion has 0 magnitude");
    }
    m_real      = m_real / mag;
    m_imaginary = Triple(m_imaginary.m_x / mag, m_imaginary.m_y / mag, m_imaginary.m_z / mag);
}

template <int T>
float
QuaternionT<T>::magnitude() const
{
    return sqrt(magnitude_squared());
}

template <int T>
float
QuaternionT<T>::magnitude_squared() const
{
    return m_real * m_real + m_imaginary.m_x * m_imaginary.m_x + m_imaginary.m_y * m_imaginary.m_y +
           m_imaginary.m_z * m_imaginary.m_z;
}

template class QuaternionT<0>;
template class QuaternionT<1>;

}  // namespace Math
}  // namespace Dubious