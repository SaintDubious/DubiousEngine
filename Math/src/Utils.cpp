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

//////////////////////////////////////////////////////////////
float ToRadians( float Degrees )
{
    return PI * Degrees/180.0f;
}

//////////////////////////////////////////////////////////////
float ToDegrees( float Radians )
{
    return Radians * 180.0f / PI;
}

}
}
