#include "Vector.h"
#include "UnitVector.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

//////////////////////////////////////////////////////////////
template<int T>
VectorT<T>::VectorT( const UnitVectorT<T>& U)
    : m_Coords( U.X(), U.Y(), U.Z() )
{
}

//////////////////////////////////////////////////////////////
template<int T>
float VectorT<T>::LengthSquared() const
{
    return m_Coords.m_X*m_Coords.m_X + m_Coords.m_Y*m_Coords.m_Y + m_Coords.m_Z*m_Coords.m_Z;
}

//////////////////////////////////////////////////////////////
template<int T>
float VectorT<T>::Length() const
{
    return sqrt(LengthSquared());
}

template class VectorT<0>;
template class VectorT<1>;

}}