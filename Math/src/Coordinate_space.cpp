#include "Coordinate_space.h"
#include "Vector_math.h"
#include "Quaternion.h"

#include <tuple>

namespace Dubious {
namespace Math {

Coordinate_space::Coordinate_space(const Point& p, const Unit_quaternion& r)
    : m_position(p), m_rotation(r)
{
}

void
Coordinate_space::get_matrix(float matrix[16]) const
{
    m_rotation.get_matrix(matrix);
}

std::tuple<Unit_vector, Unit_vector, Unit_vector>
Coordinate_space::get_axes() const
{
    float matrix[16];
    m_rotation.get_matrix(matrix);

    using namespace Dubious::Math::Matrix_index;
    return std::make_tuple(Unit_vector(matrix[_11], matrix[_21], matrix[_31]),
                           Unit_vector(matrix[_12], matrix[_22], matrix[_32]),
                           Unit_vector(matrix[_13], matrix[_23], matrix[_33]));
}

void
Coordinate_space::translate(const Vector& diff)
{
    m_position = m_position + diff;
}

void
Coordinate_space::translate(const Local_vector& diff)
{
    m_position = m_position + transform(diff);
}

void
Coordinate_space::rotate(const Unit_quaternion& diff)
{
    // Our stored m_rotation is an offset from the global coordinate space. If we want to rotate
    // against the global space (diff) we can't apply it to our existing m_rotation because
    // that's already put us into a local coordinate space. So we start by applying diff to
    // the global space, and then apply out m_rotation to get our new m_rotation
    m_rotation = diff * m_rotation;
}

void
Coordinate_space::rotate(const Local_unit_quaternion& diff)
{
    // We want to directly multiply the rotation and the diff, but our type mangling makes this
    // impossible. So we create a Unit_quaternion that's a direct copy of the supplied
    // Local_unit_quaternion and use that.
    m_rotation =
        m_rotation * Unit_quaternion(diff.m_w, Vector(diff.m_v.x(), diff.m_v.y(), diff.m_v.z()));
}

Vector
Coordinate_space::transform(const Local_vector& v) const
{
    // https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
    // I have to do some (hacky) direct converstions between local and
    // global space... but that's because this function is meant to do that
    // exact conversion. I do it in global space because that's how
    // m_rotation is stored.
    Quaternion diff(0, Vector(v.x(), v.y(), v.z()));
    Quaternion rotated = m_rotation * diff * m_rotation.conjugate();
    return rotated.v();
}

Local_vector
Coordinate_space::transform(const Vector& v) const
{
    // The inverse of Vector Coordinate_space::transform(const Local_vector& v)
    Quaternion diff(0, v);
    Quaternion rotated = m_rotation.conjugate() * diff * m_rotation;
    return Local_vector(rotated.v().x(), rotated.v().y(), rotated.v().z());
}

Point
Coordinate_space::transform(const Local_point& p) const
{
    return m_position + transform(Math::to_vector(p));
}

Local_point
Coordinate_space::transform(const Point& p) const
{
    return Math::to_point(transform(p - m_position));
}

bool
operator==(const Coordinate_space& a, const Coordinate_space& b)
{
    return a.position() == b.position() && a.rotation() == b.rotation();
}

bool
operator!=(const Coordinate_space& a, const Coordinate_space& b)
{
    return !(a == b);
}

std::ostream&
operator<<(std::ostream& o, const Coordinate_space& c)
{
    o << "P: " << c.position() << " R: " << c.rotation();
    return o;
}

}  // namespace Math
}  // namespace Dubious