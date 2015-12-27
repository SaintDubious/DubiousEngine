#include "CoordinateSpace.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Math {

//////////////////////////////////////////////////////////////
void CoordinateSpace::GetMatrix( float Matrix[16] ) const
{
    m_Rotation.GetMatrix( Matrix );
}

//////////////////////////////////////////////////////////////
void CoordinateSpace::GetAxes( UnitVector& X, UnitVector& Y, UnitVector& Z ) const
{
    float Matrix[16];
    m_Rotation.GetMatrix( Matrix );

    using namespace Dubious::Math::MatrixIndex;
    X = UnitVector( Matrix[_11], Matrix[_21], Matrix[_31] );
    Y = UnitVector( Matrix[_12], Matrix[_22], Matrix[_32] );
    Z = UnitVector( Matrix[_13], Matrix[_23], Matrix[_33] );
}

//////////////////////////////////////////////////////////////
void CoordinateSpace::Translate( const Vector& Diff )
{
    m_Position = m_Position + Diff;
}

//////////////////////////////////////////////////////////////
void CoordinateSpace::Translate( const LocalVector& Diff )
{
    m_Position = m_Position + Transform(Diff);
}

//////////////////////////////////////////////////////////////
void CoordinateSpace::Rotate( const Quaternion& Diff )
{
    m_Rotation = Diff * m_Rotation;
}

//////////////////////////////////////////////////////////////
void CoordinateSpace::Rotate( const LocalQuaternion& Diff )
{
    // We want to directly multiple the Diff and the Rotation,
    // but our type mangling makes this impossible. So we just
    // create a Quaternion that's a direct copy of the 
    // LocalQuaternion
    m_Rotation = m_Rotation * Quaternion( Diff.m_Real, Diff.m_Imaginary );
}

//////////////////////////////////////////////////////////////
Vector CoordinateSpace::Transform( const LocalVector& V ) const
{
    // https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
    Quaternion QDiff( 0, Triple( V.X(), V.Y(), V.Z() ) );
    Quaternion QRotated = m_Rotation * QDiff * m_Rotation.Conjugate();
    return Vector( QRotated.m_Imaginary.m_X, QRotated.m_Imaginary.m_Y, QRotated.m_Imaginary.m_Z );
}

//////////////////////////////////////////////////////////////
LocalVector CoordinateSpace::Transform( const Vector& V ) const
{
    // https://en.wikipedia.org/wiki/Quaternions_and_spatial_rotation
    Quaternion QDiff( 0, Triple( V.X(), V.Y(), V.Z() ) );
    Quaternion QRotated = m_Rotation.Conjugate() * QDiff * m_Rotation;
    return LocalVector( QRotated.m_Imaginary.m_X, QRotated.m_Imaginary.m_Y, QRotated.m_Imaginary.m_Z );
}

bool operator== ( const CoordinateSpace& A, const CoordinateSpace& B )
{
    return A.m_Position == B.m_Position &&
           A.m_Rotation == B.m_Rotation;
}

}}