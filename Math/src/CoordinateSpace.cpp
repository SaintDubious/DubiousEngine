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
    // The trick here is to first rotate the Passed in Vector
    // by the rotation of this coordinate space. That will
    // transform the vector into this space. Then you can 
    // simply add it to the existing position
    // http://math.stackexchange.com/questions/40164/how-do-you-rotate-a-vector-by-a-unit-quaternion
    Quaternion QDiff( 0, Triple( Diff.X(), Diff.Y(), Diff.Z() ) );
    Quaternion QRotated = m_Rotation * QDiff * m_Rotation.Conjugate();
    Vector Rotated( QRotated.m_Imaginary.m_X, QRotated.m_Imaginary.m_Y, QRotated.m_Imaginary.m_Z );

    m_Position = m_Position + Rotated;
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

}}