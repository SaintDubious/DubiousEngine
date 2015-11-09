#include "CppUnitTest.h"

#include <UnitVector.h>
#include <Utils.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using Dubious::Math::UnitVector;
using Dubious::Math::Equals;

namespace MathTest {

TEST_CLASS(UnitVectorTest)
{
public:

    TEST_METHOD(UnitVectorConstruction)
    {
//        Assert::ExpectException<std::runtime_error>( UnitVector U(0,0,0) );

        UnitVector P(1, 2, 3);
        Assert::IsTrue(Equals(P.X(), 1));
        Assert::IsTrue(Equals(P.Y(), 2));
        Assert::IsTrue(Equals(P.Z(), 3));

        UnitVector Q(P);
        Assert::IsTrue(Equals(Q.X(), 1));
        Assert::IsTrue(Equals(Q.Y(), 2));
        Assert::IsTrue(Equals(Q.Z(), 3));
    }

};

}