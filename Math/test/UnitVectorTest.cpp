#include "CppUnitTest.h"

#include <UnitVector.h>
#include <Vector.h>
#include <Utils.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using Dubious::Math::UnitVector;
using Dubious::Math::Vector;
using Dubious::Math::Equals;

namespace MathTest {

TEST_CLASS(UnitVectorTest)
{
public:

    TEST_METHOD(UnitVectorConstruction)
    {
        UnitVector P(1, 2, 3);
        Assert::IsTrue(Equals(P.X(), 0.267261237f));
        Assert::IsTrue(Equals(P.Y(), 0.534522474f));
        Assert::IsTrue(Equals(P.Z(), 0.801783681f));

        UnitVector Q(P);
        Assert::IsTrue(Equals(Q.X(), 0.267261237f));
        Assert::IsTrue(Equals(Q.Y(), 0.534522474f));
        Assert::IsTrue(Equals(Q.Z(), 0.801783681f));

        Vector R( 1, 2, 3 );
        UnitVector S(R);
        Assert::IsTrue(Equals(S.X(), 0.267261237f));
        Assert::IsTrue(Equals(S.Y(), 0.534522474f));
        Assert::IsTrue(Equals(S.Z(), 0.801783681f));

        try {
            UnitVector U(0, 0, 0);
            Assert::Fail(L"Empty Unit Vector did not throw std::runtime_error");
        }
        catch (const std::runtime_error&) {
        }
        catch (...) {
            Assert::Fail(L"Empty Unit Vector threw wrong exception type");
        }

        try {
            Vector V;
            UnitVector U(V);
            Assert::Fail(L"Unit Vector from empty Vector did not throw std::runtime_error");
        }
        catch (const std::runtime_error&) {
        }
        catch (...) {
            Assert::Fail(L"Unit Vector from empty Vector thre wrong exception type");
        }
    }

    TEST_METHOD(UnitVectorOperators)
    {
        UnitVector P(1, 2, 3);
        UnitVector Q(1, 1, 1);

        Assert::IsTrue( Q != P );

        Q = P;
        Assert::IsTrue(Equals(Q.X(), 0.267261237f));
        Assert::IsTrue(Equals(Q.Y(), 0.534522474f));
        Assert::IsTrue(Equals(Q.Z(), 0.801783681f));

        Assert::IsTrue( Q == P );

    }

};

}