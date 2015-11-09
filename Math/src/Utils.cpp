#include "Utils.h"

#include <cmath>
#include <limits>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

//////////////////////////////////////////////////////////////
bool Equals( float A, float B )
{
	return fabs(A-B) < std::numeric_limits<float>::round_error();
}

}
}
