#include "UnitVector.h"
#include "Vector.h"

//////////////////////////////////////////////////////////////
using Dubious::Math::UnitVectorT;
using Dubious::Math::VectorT;

//////////////////////////////////////////////////////////////
template <int T>
UnitVectorT<T>::UnitVectorT( float X, float Y, float Z)
{
    float Length = X*X + Y*Y + Z*Z;
    if (Equals(Length, 0)) {
        throw std::runtime_error( "UnitVector created with 0 length" );
    }
    m_Coords = Triple( X/Length, Y/Length, Z/Length );
}

template class UnitVectorT<0>;
