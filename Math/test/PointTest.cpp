#include "CppUnitTest.h"

#include <Point.h>
#include <Utils.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using Dubious::Math::Point;
using Dubious::Math::Equals;

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
    }

};

}