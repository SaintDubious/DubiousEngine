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

////////////////////////////////////////////////////////////////////////////////////
template <int T>
void QuaternionT<T>::GetMatrix( float Matrix[16] ) const
{
    float 	xx( m_Imaginary.m_X * m_Imaginary.m_X );
    float 	yy( m_Imaginary.m_Y * m_Imaginary.m_Y );
    float 	zz( m_Imaginary.m_Z * m_Imaginary.m_Z );

    using namespace MatrixIndex;

    Matrix[_11] = 1.0f - 2.0f * ( yy + zz );
    Matrix[_21] = 2.0f * ( m_Imaginary.m_X * m_Imaginary.m_Y + m_Real * m_Imaginary.m_Z );
    Matrix[_31] = 2.0f * ( m_Imaginary.m_X * m_Imaginary.m_Z - m_Real * m_Imaginary.m_Y );
    Matrix[_41] = 0.0;

    Matrix[_12] = 2.0f * ( m_Imaginary.m_X * m_Imaginary.m_Y - m_Real * m_Imaginary.m_Z );
    Matrix[_22] = 1.0f - 2.0f * ( xx + zz );
    Matrix[_32] = 2.0f * ( m_Imaginary.m_Y * m_Imaginary.m_Z + m_Real * m_Imaginary.m_X );
    Matrix[_42] = 0.0;

    Matrix[_13] = 2.0f * ( m_Imaginary.m_X * m_Imaginary.m_Z + m_Real * m_Imaginary.m_Y );
    Matrix[_23] = 2.0f * ( m_Imaginary.m_Y * m_Imaginary.m_Z - m_Real * m_Imaginary.m_X );
    Matrix[_33] = 1.0f - 2.0f * ( xx + yy );
    Matrix[_43] = 0.0;

    Matrix[_14] = 0.0;
    Matrix[_24] = 0.0;
    Matrix[_34] = 0.0;
    Matrix[_44] = 1.0;
}

template class QuaternionT<0>;
