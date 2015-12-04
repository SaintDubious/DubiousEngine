#ifndef INCLUDED_MATH_VECTORMATH
#define INCLUDED_MATH_VECTORMATH

#include "Vector.h"

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

}
}

#endif

