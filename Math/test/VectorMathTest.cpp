#include "CppUnitTest.h"

#include <VectorMath.h>
#include <Utils.h>
#include <UnitVector.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using Dubious::Math::Vector;
using Dubious::Math::UnitVector;
using Dubious::Math::Equals;
using Dubious::Math::CrossProduct;
using Dubious::Math::ToRadians;

namespace MathTest {

TEST_CLASS(VectorMathTest)
{
public:

    TEST_METHOD(CrossProductTest)
    {
        // XY Plane tests
        CrossTest( Vector( 1, 0, 0 ), Vector( 0, 1, 0 ), UnitVector( 0, 0, 1 ), 90 );
        CrossTest( Vector( 0, 1, 0 ), Vector( 1, 0, 0 ), UnitVector( 0, 0, -1 ), 90 );
        CrossTest( Vector( 1, 1, 0 ), Vector( -1, 1, 0 ), UnitVector( 0, 0, 1 ), 90 );
        CrossTest( Vector( 1, 1, 0 ), Vector( 0, 1, 0 ), UnitVector( 0, 0, 1 ), 45 );
        CrossTest( Vector( -1, -1, 0 ), Vector( -1, 0, 0 ), UnitVector( 0, 0, -1 ), 45 );
        CrossTest( Vector( -1.732050807f, -1, 0 ), Vector( -1, -1.732050807f, 0 ), UnitVector( 0, 0, 1 ), 30 );
        CrossTest( Vector( 0, -1, 0 ), Vector( 1.732050807f, -1, 0 ), UnitVector( 0, 0, 1 ), 60 );
        // XZ Plane tests
        CrossTest( Vector( 1, 0, 1 ), Vector( -1, 0, 1 ), UnitVector( 0, -1, 0 ), 90 );
        CrossTest( Vector( 1, 0, 1 ), Vector( 0, 0, 1 ), UnitVector( 0, -1, 0 ), 45 );
        CrossTest( Vector( -1, 0, -1 ), Vector( -1, 0, 0 ), UnitVector( 0, 1, 0 ), 45 );
        CrossTest( Vector( -1.732050807f, 0, -1 ), Vector( -1, 0, -1.732050807f ), UnitVector( 0, -1, 0 ), 30 );
        // YZ Plane tests
        CrossTest( Vector( 0, 1, 1 ), Vector( 0, -1, 1 ), UnitVector( 1, 0, 0 ), 90 );
        CrossTest( Vector( 0, 1, 1 ), Vector( 0, 0, 1 ), UnitVector( 1, 0, 0 ), 45 );
        CrossTest( Vector( 0, -1, -1 ), Vector( 0, -1, 0 ), UnitVector( -1, 0, 0 ), 45 );
        CrossTest( Vector( 0, -1.732050807f, -1 ), Vector( 0, -1, -1.732050807f ), UnitVector( 1, 0, 0 ), 30 );
        // Off Plane tests
        CrossTest( Vector( 1, 1, 1 ), Vector( -1, 1, 1 ), UnitVector( 0, -1, 1 ), 70.5287768017f );
    }

    TEST_METHOD(DotProductTest)
    {
        // XY Plane tests
        DotTest( Vector( 1, 0, 0 ), Vector( 0, 1, 0 ), 90 );
        DotTest( Vector( 0, 1, 0 ), Vector( 1, 0, 0 ), 90 );
        DotTest( Vector( 1, 1, 0 ), Vector( -1, 1, 0 ), 90 );
        DotTest( Vector( 1, 1, 0 ), Vector( 0, 1, 0 ), 45 );
        DotTest( Vector( -1, -1, 0 ), Vector( -1, 0, 0 ), 45 );
        DotTest( Vector( -1.732050807f, -1, 0 ), Vector( -1, -1.732050807f, 0 ), 30 );
        DotTest( Vector( 0, -1, 0 ), Vector( 1.732050807f, -1, 0 ), 60 );
        // XZ Plane tests
        DotTest( Vector( 1, 0, 1 ), Vector( -1, 0, 1 ), 90 );
        DotTest( Vector( 1, 0, 1 ), Vector( 0, 0, 1 ), 45 );
        DotTest( Vector( -1, 0, -1 ), Vector( -1, 0, 0 ), 45 );
        DotTest( Vector( -1.732050807f, 0, -1 ), Vector( -1, 0, -1.732050807f ), 30 );
        // YZ Plane tests
        DotTest( Vector( 0, 1, 1 ), Vector( 0, -1, 1 ), 90 );
        DotTest( Vector( 0, 1, 1 ), Vector( 0, 0, 1 ), 45 );
        DotTest( Vector( 0, -1, -1 ), Vector( 0, -1, 0 ), 45 );
        DotTest( Vector( 0, -1.732050807f, -1 ), Vector( 0, -1, -1.732050807f ), 30 );
        // Off Plane tests
        DotTest( Vector( 1, 1, 1 ), Vector( -1, 1, 1 ), 70.5287768017f );
    }

private:

    void CrossTest( const Vector& V1, const Vector& V2, const UnitVector& Dir, float Angle )
    {
        Vector Result = CrossProduct( V1, V2 );
        float ResultLength = Result.Length();
        float V1Length = V1.Length();
        float V2Length = V2.Length();
        float SinAngle = sinf(ToRadians(Angle));
        float ExpectedLength = V1Length*V2Length*SinAngle;
        Assert::IsTrue( Equals( ResultLength, ExpectedLength ) );
        UnitVector ResultDir( Result );
        Assert::IsTrue( Dir == ResultDir );
    }

    void DotTest( const Vector& V1, const Vector& V2, float Angle )
    {
        float Result = DotProduct( V1, V2 );
        float V1Length = V1.Length();
        float V2Length = V2.Length();
        float CosAngle = cosf(ToRadians(Angle));
        Assert::IsTrue( Equals( Result, (V1Length*V2Length*CosAngle)) );
    }
};

}