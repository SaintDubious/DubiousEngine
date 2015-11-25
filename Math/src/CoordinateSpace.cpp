#include "CoordinateSpace.h"

//////////////////////////////////////////////////////////////
using Dubious::Math::CoordinateSpaceT;
using Dubious::Math::UnitVector;

template <int T>
void CoordinateSpaceT<T>::GetAxes( UnitVector& X, UnitVector& Y, UnitVector& Z ) const
{
    float Matrix[16];
    m_Rotation.GetMatrix( Matrix );

    using namespace Dubious::Math::MatrixIndex;
    X = UnitVector( Matrix[_11], Matrix[_21], Matrix[_31] );
    Y = UnitVector( Matrix[_12], Matrix[_22], Matrix[_32] );
    Z = UnitVector( Matrix[_13], Matrix[_23], Matrix[_33] );
}

template class CoordinateSpaceT<0>;

