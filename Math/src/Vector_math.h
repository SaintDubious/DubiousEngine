#ifndef INCLUDED_MATH_VECTORMATH
#define INCLUDED_MATH_VECTORMATH

#include "Vector.h"
#include "Unit_vector.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

//////////////////////////////////////////////////////////////
template <int T>
VectorT<T> cross_product( const VectorT<T>& a, const VectorT<T>& b )
{
    return VectorT<T>( (a.y()*b.z()) - (a.z()*b.y()), 
        (a.z()*b.x()) - (a.x()*b.z()), 
        (a.x()*b.y()) - (a.y()*b.x()));
}

//////////////////////////////////////////////////////////////
template <int T>
float dot_product( const VectorT<T>& a, const VectorT<T>& b )
{
    return a.x()*b.x() + a.y()*b.y() + a.z()*b.z();
}

//////////////////////////////////////////////////////////////
template <int T>
float dot_product( const Unit_vectorT<T>& A, const Unit_vectorT<T>& B )
{
    return a.x()*b.x() + a.y()*b.y() + a.z()*b.z();
}

}
}

#endif

