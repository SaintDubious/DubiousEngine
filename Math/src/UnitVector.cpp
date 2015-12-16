#include "UnitVector.h"
#include "Vector.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

//////////////////////////////////////////////////////////////
template <int T>
UnitVectorT<T>::UnitVectorT( float X, float Y, float Z)
{
    float Length = sqrt(X*X + Y*Y + Z*Z);
    if (Equals(Length, 0)) {
        throw std::runtime_error( "UnitVector created with 0 length" );
    }
    m_Coords = Triple( X/Length, Y/Length, Z/Length );
}

//////////////////////////////////////////////////////////////
template<int T>
UnitVectorT<T>::UnitVectorT(const VectorT<T>& V)
{
    float Length = V.Length();
    if (Equals(Length, 0)) {
        throw std::runtime_error("UnitVector created from 0 length Vector");
    }
    m_Coords = Triple(V.X() / Length, V.Y() / Length, V.Z() / Length);
}

template class UnitVectorT<0>;
template class UnitVectorT<1>;

}}