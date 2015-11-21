#ifndef INCLUDED_MATH_COORDINATESPACE
#define INCLUDED_MATH_COORDINATESPACE

#include "Point.h"
#include "Quaternion.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

/// @brief Representation of a position and rotation
///	
///	The CoordinateSpace is meant to represent a position and rotation.  It allows you to
///	hide the complexities of world versus local rotation and position.  You can easily specify
///	that you want to move along the global X OR local X axis.  Likewise you can rotate around
///	the global OR local Z axis.  
///
///	The system uses a "Right Handed" coordinate space, the same as OpenGL and Bullet Physics. 
///	This means that as you are reading this, positive X is to your right, positive Y is up,
///	and positive Z is going from this text to your eye.  If you hold up your right hand and 
///	place your thumb along positive X and your index finger along positive Y then your middle
///	finger is pointing along positive Z (at you).
template <int T>
class CoordinateSpaceT
{
public:
    /// @brief Default constructor
    ///
    /// The default constructor creates a coordinate space centered at 
    /// (0,0,0) with the +Y axis as Up and +Z as Forward, etc etc
    CoordinateSpaceT() 
    {}

private:
    PointT<T>           m_Position;
    QuaternionT<T>      m_Rotation;
};

}
}

#endif
