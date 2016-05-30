#ifndef INCLUDED_MATH_COORDINATESPACE
#define INCLUDED_MATH_COORDINATESPACE

#include "Point.h"
#include "Quaternion.h"

namespace Physics_test { class Collision_solver_test; }

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

/// @brief Representation of a position and rotation
///	
///	The Coordinate_space is meant to represent a position and rotation.  It allows you to
///	hide the complexities of world versus local rotation and position.  You can easily specify
///	that you want to move along the global X OR local X axis.  Likewise you can rotate around
///	the global OR local Z axis.  
///
///	The system uses a "Right Handed" coordinate space, the same as OpenGL and Bullet Physics. 
///	This means that as you are reading this, positive X is to your right, positive Y is up,
///	and positive Z is going from this text to your eye.  If you hold up your right hand and 
///	place your thumb along positive X and your index finger along positive Y then your middle
///	finger is pointing along positive Z (at you).
class Coordinate_space {
public:
    Coordinate_space() = default;

    /// @brief Get 3 axis
    ///
    /// It's common to want the X, Y, and Z axis that make up 
    /// a coordinate system
    /// @returns a tuple with X, Y, and Z Unit_vectors
    std::tuple<Unit_vector,Unit_vector,Unit_vector> get_axes() const;

    /// @brief Get the OpenGL rotation Matrix
    ///
    /// Get the rotation matrix from the Quaternion. This is
    /// directly usable by openGL
    /// @param matrix - [out] The rotation matrix
    void                get_matrix( float matrix[16] ) const;

    /// @brief Position accessor
    const Point&        position() const { return m_position; }
    Point&              position() { return m_position; }

    /// @brief Rotation accessor
    const Quaternion&   rotation() const { return m_rotation; }
    Quaternion&         rotation() { return m_rotation; }

    /// @brief Move by the amount specified
    ///
    /// This version takes a global vector, therefore it moves
    /// relative to global space.
    /// @param diff - [in] The amount to move
    void                translate( const Vector& diff );

    /// @brief Move by the amount specified
    ///
    /// This version takes a local vector, therefore it moves
    /// relative to local space.
    /// @param diff - [in] The amount to move
    void                translate( const Local_vector& diff );

    /// @brief Rotate the coordinate space
    ///
    /// This version takes a global quaternion, therefore it
    /// rotates relative to global space
    /// @param diff - [in] The amount to rotate
    void                rotate( const Quaternion& diff );

    /// @brief Rotate the coordinate space
    ///
    /// This version takes a local quaternion, therefore it
    /// rotates relative to local space
    /// @param diff - [in] The amount to rotate
    void                rotate( const Local_quaternion& diff );

    /// @brief Converts the local vector to the global coord space.
    ///
    /// @param v - [in] a vector in local coordinate space
    /// @returns The same vector in world coordinate space
    Vector		        transform( const Local_vector& v ) const;

    /// @brief Converts the vector to the local coord space
    ///
    /// @param v - [in] a vector in world coordinate space
    /// @returns The same vector in local coordinate space
    Local_vector	    transform( const Vector& v ) const;

private:    
    friend class Physics_test::Collision_solver_test;

    friend bool         operator== ( const Coordinate_space& a, const Coordinate_space& b );
    friend std::ostream& operator<<(std::ostream& o, const Coordinate_space& c);

    Coordinate_space( const Point& p, const Quaternion& r );

    Point               m_position;
    Quaternion          m_rotation;
};

bool operator== ( const Coordinate_space& a, const Coordinate_space& b );
std::ostream& operator<<(std::ostream& o, const Coordinate_space& c);

}
}

#endif
