#ifndef INCLUDED_MATH_QUATERNION
#define INCLUDED_MATH_QUATERNION

#include "Triple.h"

namespace MathTest { class QuaternionTest; }

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

template <int T> class UnitVectorT;

/// @brief Unit Quaternion
///
/// Everyone's favorite 4 dimensional, completely mysterious 
/// mathmatical construct. The Unit Quaternion is meant to represent
/// rotations in this game engine. There are a million quaternion
/// resources online. Here are some I'm fond of:
/// http://www.cprogramming.com/tutorial/3d/quaternions.html
/// http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/slerp/
/// http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToAngle/index.htm
template <int T>
class QuaternionT
{
public:
    /// @brief Default Constructor
    ///
    /// The default constructor will create a rotation that represents
    /// Axis aligned with global X, Y, and Z. In other words this will
    /// create exactly what you'd expect for a default constructor
    QuaternionT()
        : m_Real( 1.0f )
    {}

    /// @brief Construct from axis and angle
    ///
    /// This is a more useful constructor where you can specify an
    /// axis and an angle around that axis. This is a right hand coordinate
    /// system, so the angle is counter-clockwise rotation around the
    /// axis
    QuaternionT( const UnitVectorT<T>& Axis, float Angle );

    /// @brief Default copy constructor
    QuaternionT( const QuaternionT& ) = default;

    /// @brief Default destructor
    ~QuaternionT() = default;

    /// @brief Default assignment operator
    QuaternionT& operator=( const QuaternionT& ) = default;

private:
    friend class MathTest::QuaternionTest;
    float               m_Real;
    Triple              m_Imaginary;
};

typedef QuaternionT<0> Quaternion;

}
}

#endif
