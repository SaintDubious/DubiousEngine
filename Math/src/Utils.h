#ifndef INCLUDED_MATH_UTILS
#define INCLUDED_MATH_UTILS

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math	{

/// @brief Tests equality of two floating point numbers.  
///
/// Returns true if they are "close enough" to being equals.  Basically
///	does an == that takes rounding into account.
/// @param A - [in] The first number
/// @param B - [in] The second number
/// @returns True if the numbers are "close enough" to being equal
bool Equals( float A, float B );

}
}

#endif