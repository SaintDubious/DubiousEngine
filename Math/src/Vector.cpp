#include "Vector.h"
#include "Unit_vector.h"

namespace Dubious {
namespace Math {

template<int T>
VectorT<T>::VectorT( const Unit_vectorT<T>& u)
    : m_coords( u.x(), u.y(), u.z() )
{
}

template<int T>
float VectorT<T>::length_squared() const
{
    return m_coords.m_x*m_coords.m_x + m_coords.m_y*m_coords.m_y + m_coords.m_z*m_coords.m_z;
}

template<int T>
float VectorT<T>::length() const
{
    return sqrt(length_squared());
}

template<int T>
VectorT<T>& VectorT<T>::operator+=( const VectorT<T>& rhs )
{
    m_coords += rhs.m_coords;
    return *this;
}

template<int T>
VectorT<T>& VectorT<T>::operator-=( const VectorT<T>& rhs )
{
    m_coords -= rhs.m_coords;
    return *this;
}

template class VectorT<0>;
template class VectorT<1>;

}}