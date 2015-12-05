#include "CppUnitTest.h"

#include <Vector.h>
#include <UnitVector.h>
#include <Utils.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using Dubious::Math::Vector;
using Dubious::Math::UnitVector;
using Dubious::Math::Equals;

namespace MathTest {

TEST_CLASS(VectorTest)
{
public:

    TEST_METHOD(VectorConstruction)
    {
        Vector O;
        Assert::IsTrue(Equals(O.X(), 0));
        Assert::IsTrue(Equals(O.Y(), 0));
        Assert::IsTrue(Equals(O.Z(), 0));

        Vector P(1, 2, 3);
        Assert::IsTrue(Equals(P.X(), 1));
        Assert::IsTrue(Equals(P.Y(), 2));
        Assert::IsTrue(Equals(P.Z(), 3));

        Vector Q(P);
        Assert::IsTrue(Equals(Q.X(), 1));
        Assert::IsTrue(Equals(Q.Y(), 2));
        Assert::IsTrue(Equals(Q.Z(), 3));

        UnitVector U( 1, 2, 3 );
        Vector V( U );
        Assert::IsTrue(Equals(V.X(), 0.267261237f));
        Assert::IsTrue(Equals(V.Y(), 0.534522474f));
        Assert::IsTrue(Equals(V.Z(), 0.801783681f));
    }

    TEST_METHOD(VectorOperators)
    {
        Vector P(1, 2, 3);
        Vector Q;
        Q = P;
        Assert::IsTrue(Equals(Q.X(), 1));
        Assert::IsTrue(Equals(Q.Y(), 2));
        Assert::IsTrue(Equals(Q.Z(), 3));

        Vector R( 10, 20, 30 );
        Q = R+P;
        Assert::IsTrue(Equals(Q.X(), 11));
        Assert::IsTrue(Equals(Q.Y(), 22));
        Assert::IsTrue(Equals(Q.Z(), 33));
    }

    TEST_METHOD(VectorLength)
    {
        Vector X( 1, 2, 3 );
        Assert::IsTrue(Equals(X.LengthSquared(), 14));
        Assert::IsTrue(Equals(X.Length(), sqrt(14.0f)));
    }

};

}