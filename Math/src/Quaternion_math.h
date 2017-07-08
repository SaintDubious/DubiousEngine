#ifndef INCLUDED_MATH_QUATERNION_MATH
#define INCLUDED_MATH_QUATERNION_MATH

#include "Unit_quaternion.h"
#include "Vector_math.h"

#include <algorithm>

namespace Dubious {
namespace Math {

template <int T>
float
dot_product(const Unit_quaternionT<T>& a, const Unit_quaternionT<T>& b)
{
    return a.w() * b.w() + dot_product(a.v(), b.v());
}

// https://en.wikipedia.org/wiki/Slerp
template <int T>
Unit_quaternionT<T>
slerp(Unit_quaternionT<T> a, const Unit_quaternionT<T>& b, float t)
{
    float        dot           = dot_product(a, b);
    const double DOT_THRESHOLD = 0.9995;
    if (fabs(dot) > DOT_THRESHOLD) {
        // If the inputs are too close for comfort, linearly interpolate
        // and normalize the result.
        QuaternionT<T> result = a + t * (b - a);
        return Unit_quaternionT<T>(result);
    }

    // If the dot product is negative, the quaternions
    // have opposite handed-ness and slerp won't take
    // the shorter path. Fix by reversing one quaternion.
    //
    // I'm not actually sure I want this behavior. If something is turning
    // to catch another object you don't want it to suddenly switch
    // directions
    //
    //    if (dot < 0.0f) {
    //        a   = -a;
    //        dot = -dot;
    //    }
    dot           = std::min(std::max(dot, -1.0f), 1.0f);
    float theta_0 = acos(dot);    // theta_0 = angle between input quaternions
    float theta   = theta_0 * t;  // theta = angle between a and result

    Unit_quaternionT<T> c(b - (a * dot));

    return a * cos(theta) + c * sin(theta);
}

}  // namespace Math
}  // namespace Dubious

#endif
