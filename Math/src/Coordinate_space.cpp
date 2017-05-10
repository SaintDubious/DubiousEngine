#include "Coordinate_space.h"
#include "Vector_math.h"

#include <tuple>

namespace Dubious {
namespace Math {

Coordinate_space::Coordinate_space( const Point& p, const Quaternion& r )
    : m_position( p )
    , m_rotation( r )
{
}

void Coordinate_space::get_matrix( float matrix[16] ) const
{
    m_rotation.get_matrix( matrix );
}

std::tuple<Unit_vector,Unit_vector,Unit_vector> Coordinate_space::get_axes() const
{
    float matrix[16];
    m_rotation.get_matrix( matrix );

    using namespace Dubious::Math::Matrix_index;
    return std::make_tuple( Unit_vector( matrix[_11], matrix[_21], matrix[_31] ), 
                            Unit_vector( matrix[_12], matrix[_22], matrix[_32] ),
                            Unit_vector( matrix[_13], matrix[_23], matrix[_33] ) );
}

void Coordinate_space::translate( const Vector& diff )
{
    m_position = m_position + diff;
}

void Coordinate_space::translate( const Local_vector& diff )
{
    m_position = m_position + transform(diff);
}

void Coordinate_space::rotate( const Quaternion& diff )
{
    m_rotation = diff * m_rotation;
}

void Coordinate_space::rotate( const Local_quaternion& diff )
{
    // We want to directly multiple the Diff and the Rotation,
    // but our type mangling makes this impossible. So we just
    // create a Quaternion that's a direct copy of the 
    // LocalQuaternion
    m_rotation = m_rotation * Quaternion( diff.m_real, diff.m_imaginary );
}

Vector Coordinate_space::transform( const Local_vector& v ) const
{
    // https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
    Quaternion qdiff( 0, Triple( v.x(), v.y(), v.z() ) );
    Quaternion qrotated = m_rotation * qdiff * m_rotation.conjugate();
    return Vector( qrotated.m_imaginary.m_x, qrotated.m_imaginary.m_y, qrotated.m_imaginary.m_z );
}

Local_vector Coordinate_space::transform( const Vector& v ) const
{
    // https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
    Quaternion qdiff( 0, Triple( v.x(), v.y(), v.z() ) );
    Quaternion qrotated = m_rotation.conjugate() * qdiff * m_rotation;
    return Local_vector( qrotated.m_imaginary.m_x, qrotated.m_imaginary.m_y, qrotated.m_imaginary.m_z );
}

Point Coordinate_space::transform( const Local_point& p ) const
{
    return m_position + transform( Math::to_vector( p ) );
}

Local_point Coordinate_space::transform( const Point& p ) const
{
    return Math::to_point( transform( p - m_position ) );
}

bool operator== ( const Coordinate_space& a, const Coordinate_space& b )
{
    return a.m_position == b.m_position &&
           a.m_rotation == b.m_rotation;
}

std::ostream& operator<<(std::ostream& o, const Coordinate_space& c)
{
    o << "P: " << c.m_position << " R: " << c.m_rotation;
    return o;
}

}}