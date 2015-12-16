#ifndef INCLUDED_MATH_COORDINATESPACE
#define INCLUDED_MATH_COORDINATESPACE

#include "Point.h"
#include "Quaternion.h"
#include "UnitVector.h"

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
    CoordinateSpaceT() = default;

    /// @brief Default copy constructor
    CoordinateSpaceT( const CoordinateSpaceT& ) = default;

    /// @brief Default Destructor
    ~CoordinateSpaceT() = default;

    /// @brief Default assignment operator
    CoordinateSpaceT& operator=( const CoordinateSpaceT& ) = default;

    /// @brief Get 3 axis
    ///
    /// It's common to want the X, Y, and Z axis that make up 
    /// a coordinate system
    /// @param X - [in] X Axis
    /// @param Y - [in] Y Axis
    /// @param Z - [in] Z Axis
    void                GetAxes( UnitVector& X, UnitVector& Y, UnitVector& Z ) const;

    /// @brief Get the OpenGL rotation Matrix
    ///
    /// Get the rotation matrix from the Quaternion. This is
    /// directly usable by openGL
    /// @param Matrix - [in] The rotation matrix
    void                GetMatrix( float Matrix[16] ) const
    {
        m_Rotation.GetMatrix( Matrix );
    }

    /// @brief Position accessor
    const PointT<T>&    Position() const { return m_Position; }

private:
    PointT<T>           m_Position;
    QuaternionT<T>      m_Rotation;
};

typedef CoordinateSpaceT<0>  CoordinateSpace;

}
}

#endif
