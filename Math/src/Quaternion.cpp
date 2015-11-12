#include "Quaternion.h"
#include "UnitVector.h"

//////////////////////////////////////////////////////////////
using Dubious::Math::QuaternionT;
using Dubious::Math::UnitVectorT;

//////////////////////////////////////////////////////////////
template <int T>
QuaternionT<T>::QuaternionT( const UnitVectorT<T>& Axis, float Angle )
{
    float HalfSin = sinf( Angle / 2.0f );
    m_Real = cosf( Angle / 2.0f );
    m_Imaginary = Triple( Axis.X() * HalfSin, Axis.Y() * HalfSin, Axis.Z() * HalfSin );
}

template class QuaternionT<0>;
