#ifndef INCLUDED_MATH_UTILS
#define INCLUDED_MATH_UTILS

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math	{

static const float PI = 3.1415926535f;

/// @brief Tests equality of two floating point numbers.  
///
/// Returns true if they are "close enough" to being equals.  Basically
///	does an == that takes rounding into account.
/// @param A - [in] The first number
/// @param B - [in] The second number
/// @returns True if the numbers are "close enough" to being equal
bool Equals( float A, float B );

/// @brief converts degrees to radians
///
/// @param Degrees - [in] Angle in Degrees
/// @returns Angle in Radians
float ToRadians( float Degrees );

/// @brief converts radians to degrees
///
/// @param Radians - [in] Angle in Radians
/// @returns Angle in Degrees
float ToDegrees( float Radians );

}
}

#endif