#ifndef INCLUDED_MATH_VECTORMATH
#define INCLUDED_MATH_VECTORMATH

#include "Vector.h"
#include "Unit_vector.h"
#include "Point.h"

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
VectorT<T> cross_product( const Unit_vectorT<T>& a, const Unit_vectorT<T>& b )
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
float dot_product( const Unit_vectorT<T>& a, const Unit_vectorT<T>& b )
{
    return a.x()*b.x() + a.y()*b.y() + a.z()*b.z();
}

//////////////////////////////////////////////////////////////
// As much as I'd like to enforce that points and vectors are
// unique things that can't be arbitrarily converted, there
// do exists realities when points just need to be vectors.
// At least using a specific function will let me search for
// this if I come up with something cleaner in the future.
template <int T>
VectorT<T> to_vector( const PointT<T>& p )
{
    return VectorT<T>( p.x(), p.y(), p.z() );
}

//////////////////////////////////////////////////////////////
template <int T>
PointT<T> to_point( const VectorT<T>& v )
{
    return PointT<T>( v.x(), v.y(), v.z() );
}

}
}

#endif

