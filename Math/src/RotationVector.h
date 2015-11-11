#ifndef INCLUDED_MATH_ROTATIONVECTOR
#define INCLUDED_MATH_ROTATIONVECTOR

#include "UnitVector.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

/// @brief represents rotation by using a vector
///
/// A Rotation Vector is used to represent a vector around which
/// you will rotate, and an angle that represents the magnitude of 
/// the rotation. I chose to represent this as a Unit Vector and
/// angle because that is easy to convert to a Quaternion.
///
/// This engine used a right hand coordinate system (see CoordinateSpace.h)
/// which means positive rotation is counter clockwise around this
/// axis. "Right hand rule" and all that
template <int T>
class RotationVectorT
{
public:
    /// @brief Constructor
    ///
    /// Default constructor creates a rotation that is 0 degrees
    /// around the X axis.
    RotationVectorT()
        : m_Axis( 1, 0, 0 )
        , m_Angle( 0 )
    {}

    // @brief Default copy Constructor
    RotationVectorT( const RotationVectorT& ) = default;

    /// @brief Default Destructor
    ~RotationVectorT() = default;

    /// @brief Default assignment operator
    RotationVectorT& operator=( const RotationVectorT& ) = default;

    /// @brief Axis accessor
    const UnitVectorT<T> Axis() const { return m_Axis; }

    /// @brief Angle accessor
    float               Angle() const { return m_Angle; }

private:
    UnitVectorT<T>      m_Axis;
    float               m_Angle;
};

typedef RotationVectorT<0> RotationVector;
}
}

#endif