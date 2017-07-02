#ifndef INCLUDED_MATH_UTILS
#define INCLUDED_MATH_UTILS

namespace Dubious {
namespace Math {

static const float PI = 3.1415926535f;

/// @brief Tests equality of two floating point numbers.
///
/// Returns true if they are "close enough" to being equals.  Basically
///	does an == that takes rounding into account.
/// @param a - [in] The first number
/// @param b - [in] The second number
/// @returns True if the numbers are "close enough" to being equal
bool equals(float a, float b);

/// @brief converts degrees to radians
///
/// @param degrees - [in] Angle in Degrees
/// @returns Angle in Radians
float to_radians(float degrees);

/// @brief converts radians to degrees
///
/// @param radians - [in] Angle in Radians
/// @returns Angle in Degrees
float to_degrees(float radians);

}  // namespace Math
}  // namespace Dubious

#endif