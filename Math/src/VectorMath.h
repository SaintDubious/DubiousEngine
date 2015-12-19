#ifndef INCLUDED_MATH_VECTORMATH
#define INCLUDED_MATH_VECTORMATH

#include "Vector.h"
#include "UnitVector.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

//////////////////////////////////////////////////////////////
template <int T>
VectorT<T> CrossProduct( const VectorT<T>& A, const VectorT<T>& B )
{
    return VectorT<T>( (A.Y()*B.Z()) - (A.Z()*B.Y()), 
        (A.Z()*B.X()) - (A.X()*B.Z()), 
        (A.X()*B.Y()) - (A.Y()*B.X()));
}

//////////////////////////////////////////////////////////////
template <int T>
float DotProduct( const VectorT<T>& A, const VectorT<T>& B )
{
    return A.X()*B.X() + A.Y()*B.Y() + A.Z()*B.Z();
}

//////////////////////////////////////////////////////////////
template <int T>
float DotProduct( const UnitVectorT<T>& A, const UnitVectorT<T>& B )
{
    return A.X()*B.X() + A.Y()*B.Y() + A.Z()*B.Z();
}

}
}

#endif

