#include "CppUnitTest.h"

#include <Point.h>
#include <Vector.h>
#include <UnitVector.h>
#include <Utils.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Dubious::Math;

namespace MathTest {

TEST_CLASS(PointTest)
{
public:
		
	TEST_METHOD(PointConstruction)
	{
        Point O;
        Assert::IsTrue(Equals(O.X(), 0));
        Assert::IsTrue(Equals(O.Y(), 0));
        Assert::IsTrue(Equals(O.Z(), 0));
        
        Point P( 1, 2, 3 );
        Assert::IsTrue(Equals(P.X(), 1));
        Assert::IsTrue(Equals(P.Y(), 2));
        Assert::IsTrue(Equals(P.Z(), 3));

        Point Q( P );
        Assert::IsTrue(Equals(Q.X(), 1));
        Assert::IsTrue(Equals(Q.Y(), 2));
        Assert::IsTrue(Equals(Q.Z(), 3));
	}

    TEST_METHOD(PointOperators)
    {
        Point Q( 1, 2, 3 );
        Point P;

        Assert::IsTrue( P != Q );
        P = Q;
        Assert::IsTrue( P == Q );
        Assert::IsTrue(Equals(P.X(), 1));
        Assert::IsTrue(Equals(P.Y(), 2));
        Assert::IsTrue(Equals(P.Z(), 3));

        Point R( 10, 20, 30 );
        Vector Result = R-Q;
        Assert::IsTrue(Equals(Result.X(), 9));
        Assert::IsTrue(Equals(Result.Y(), 18));
        Assert::IsTrue(Equals(Result.Z(), 27));

        P = R+Result;
        Assert::IsTrue(Equals(P.X(), 19));
        Assert::IsTrue(Equals(P.Y(), 38));
        Assert::IsTrue(Equals(P.Z(), 57));

        P = R-Result;
        Assert::IsTrue(Equals(P.X(), 1));
        Assert::IsTrue(Equals(P.Y(), 2));
        Assert::IsTrue(Equals(P.Z(), 3));

        UnitVector UnitResult( Result );
        P = R+UnitResult;
        Assert::IsTrue(Equals(P.X(), 10.2672615f));
        Assert::IsTrue(Equals(P.Y(), 20.5345230f));
        Assert::IsTrue(Equals(P.Z(), 30.8017845f));

        P = R-UnitResult;
        Assert::IsTrue(Equals(P.X(), 9.73273849f));
        Assert::IsTrue(Equals(P.Y(), 19.4654770f));
        Assert::IsTrue(Equals(P.Z(), 29.1982155f));

    }

};

}