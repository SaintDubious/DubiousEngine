#include "CppUnitTest.h"

#include <CoordinateSpace.h>
#include <Point.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Dubious::Math;

namespace MathTest {

TEST_CLASS(CoordinateSpaceTest)
{
public:

    TEST_METHOD(CoordinateSpaceConstruction)
    {
        CoordinateSpace C;
        Assert::IsTrue( C.Position() == Point( 0, 0, 0 ) );
    }

    TEST_METHOD(CoordinateSpaceOperators)
    {
        CoordinateSpace A, B;
        Assert::IsTrue( A == B );
        B.Translate( Vector( 1, 0, 0 ) );
        Assert::IsFalse( A == B );
        B.Translate( Vector( -1, 0, 0 ) );
        Assert::IsTrue( A == B );
        B.Rotate( Quaternion( UnitVector( 0, 1, 0 ), ToRadians(45) ) );
        Assert::IsFalse( A == B );
        B.Translate( Vector( 1, 0, 0 ) );
        Assert::IsFalse( A == B );
    }

    TEST_METHOD(CoordinateSpaceTranslation)
    {
        CoordinateSpace C;
        Assert::IsTrue( C.Position() == Point( 0, 0, 0 ) );
        C.Translate( Vector( 1, 2, 3) );
        Assert::IsTrue( C.Position() == Point( 1, 2, 3 ) );
        C.Translate( Vector( 1, 2, 3) );
        Assert::IsTrue( C.Position() == Point( 2, 4, 6 ) );

        C.Rotate( Quaternion( UnitVector( 0, 1, 0 ), ToRadians( 90 ) ) );
        C.Translate( Vector( 1, 2, 3) );
        Assert::IsTrue( C.Position() == Point( 3, 6, 9 ) );
        C.Translate( LocalVector( 1, 2, 3) );
        Assert::IsTrue( C.Position() == Point( 6, 8, 8 ) );
    }

    TEST_METHOD(CoordinateSpaceRotation)
    {
        CoordinateSpace C;
        UnitVector X, Y, Z;
        C.GetAxes( X, Y, Z );
        Assert::IsTrue( X == UnitVector( 1, 0, 0 ) );
        Assert::IsTrue( Y == UnitVector( 0, 1, 0 ) );
        Assert::IsTrue( Z == UnitVector( 0, 0, 1 ) );
        C.Rotate( Quaternion( UnitVector( 0, 1, 0 ), ToRadians( 90 ) ) );
        C.GetAxes( X, Y, Z );
        Assert::IsTrue( X == UnitVector( 0, 0, -1 ) );
        Assert::IsTrue( Y == UnitVector( 0, 1, 0 ) );
        Assert::IsTrue( Z == UnitVector( 1, 0, 0 ) );
        C.Rotate( Quaternion( UnitVector( 0, 0, 1 ), ToRadians( 90 ) ) );
        C.GetAxes( X, Y, Z );
        Assert::IsTrue( X == UnitVector( 0, 0, -1 ) );
        Assert::IsTrue( Y == UnitVector( -1, 0, 0 ) );
        Assert::IsTrue( Z == UnitVector( 0, 1, 0 ) );

        C.Rotate( LocalQuaternion( LocalUnitVector( 0, 0, -1 ), ToRadians( 90 ) ) );
        C.GetAxes( X, Y, Z );
        Assert::IsTrue( X == UnitVector( 1, 0, 0 ) );
        Assert::IsTrue( Y == UnitVector( 0, 0, -1 ) );
        Assert::IsTrue( Z == UnitVector( 0, 1, 0 ) );
        C.Rotate( LocalQuaternion( LocalUnitVector( 0, 1, 0 ), ToRadians( 90 ) ) );
        C.GetAxes( X, Y, Z );
        Assert::IsTrue( X == UnitVector( 0, -1, 0 ) );
        Assert::IsTrue( Y == UnitVector( 0, 0, -1 ) );
        Assert::IsTrue( Z == UnitVector( 1, 0, 0 ) );
    }

    TEST_METHOD(CoordinateSpaceVectorTransform)
    {
        CoordinateSpace C;
        C.Rotate( Quaternion( Vector( 0, 1, 0 ), ToRadians( 90 ) ) );
        Vector V = C.Transform( LocalVector( 1, 1, 1 ) );
        Assert::IsTrue( V == Vector( 1, 1, -1 ) );
        LocalVector LV = C.Transform( Vector( 1, 1, 1 ) );
        Assert::IsTrue( LV == LocalVector( -1, 1, 1 ) );
    }

};

}