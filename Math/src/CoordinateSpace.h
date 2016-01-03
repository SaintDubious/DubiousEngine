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
class CoordinateSpace
{
public:
    /// @brief Default constructor
    ///
    /// The default constructor creates a coordinate space centered at 
    /// (0,0,0) with the +Y axis as Up and +Z as Forward, etc etc
    CoordinateSpace() = default;

    /// @brief Default copy constructor
    CoordinateSpace( const CoordinateSpace& ) = default;

    /// @brief Default Destructor
    ~CoordinateSpace() = default;

    /// @brief Default assignment operator
    CoordinateSpace& operator=( const CoordinateSpace& ) = default;

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
    void                GetMatrix( float Matrix[16] ) const;

    /// @brief Position accessor
    const Point&        Position() const { return m_Position; }

    /// @brief Move by the amount specified
    ///
    /// This version takes a global vector, therefore it moves
    /// relative to global space.
    /// @param Diff - [in] The amount to move
    void                Translate( const Vector& Diff );

    /// @brief Move by the amount specified
    ///
    /// This version takes a local vector, therefore it moves
    /// relative to local space.
    /// @param Diff - [in] The amount to move
    void                Translate( const LocalVector& Diff );

    /// @brief Rotate the coordinate space
    ///
    /// This version takes a global quaternion, therefore it
    /// rotates relative to global space
    /// @param Diff - [in] The amount to rotate
    void                Rotate( const Quaternion& Diff );

    /// @brief Rotate the coordinate space
    ///
    /// This version takes a local quaternion, therefore it
    /// rotates relative to local space
    /// @param Diff - [in] The amount to rotate
    void                Rotate( const LocalQuaternion& Diff );

    /// @brief Converts the local vector to the global coord space.
    ///
    /// @param V - [in] a vector in local coordinate space
    /// @returns The same vector in world coordinate space
    Vector		        Transform( const LocalVector& V ) const;

    /// @brief Converts the vector to the local coord space
    ///
    /// @param V - [in] a vector in world coordinate space
    /// @returns The same vector in local coordinate space
    LocalVector	        Transform( const Vector& V ) const;

private:
    
    friend bool         operator== ( const CoordinateSpace& A, const CoordinateSpace& B );

    Point               m_Position;
    Quaternion          m_Rotation;
};

bool operator== ( const CoordinateSpace& A, const CoordinateSpace& B );

//////////////////////////////////////////////////////////////
inline std::ostream& operator<<(std::ostream& o, const CoordinateSpace& C)
{
    o << "Q: " << "P: ";
    return o;
}

}
}

#endif
