#include "Utils.h"

#include <cmath>
#include <limits>

namespace Dubious {
namespace Math {

bool equals( float a, float b )
{
    return fabs(a-b) < 0.000001;
}

float to_radians( float degrees )
{
    return PI * degrees/180.0f;
}

float to_degrees( float radians )
{
    return radians * 180.0f / PI;
}

}
}
