#include "Unit_vector.h"
#include "Vector.h"

namespace Dubious {
namespace Math {

template <int T>
Unit_vectorT<T>::Unit_vectorT( float x, float y, float z)
{
    float length = sqrt(x*x + y*y + z*z);
    if (equals(length, 0)) {
        throw std::runtime_error( "Unit_vectorT created with 0 length" );
    }
    m_coords = Triple( x/length, y/length, z/length );
}

template<int T>
Unit_vectorT<T>::Unit_vectorT(const VectorT<T>& v)
{
    float length = v.length();
    if (equals(length, 0)) {
        throw std::runtime_error("Unit_vectorT created from 0 length Vector");
    }
    m_coords = Triple(v.x() / length, v.y() / length, v.z() / length);
}

template class Unit_vectorT<0>;
template class Unit_vectorT<1>;

}}