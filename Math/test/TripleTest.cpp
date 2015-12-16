#include "CppUnitTest.h"

#include <Triple.h>
#include <Utils.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Dubious::Math;

namespace MathTest {

TEST_CLASS(TripleTest)
{
public:

    TEST_METHOD(TripleConstruction)
    {
        Triple P(1, 2, 3);
        Assert::IsTrue(Equals(P.m_X, 1));
        Assert::IsTrue(Equals(P.m_Y, 2));
        Assert::IsTrue(Equals(P.m_Z, 3));

        Triple Q(P);
        Assert::IsTrue(Equals(Q.m_X, 1));
        Assert::IsTrue(Equals(Q.m_Y, 2));
        Assert::IsTrue(Equals(Q.m_Z, 3));

        Triple R;
        Assert::IsTrue(Equals(R.m_X, 0));
        Assert::IsTrue(Equals(R.m_Y, 0));
        Assert::IsTrue(Equals(R.m_Z, 0));

        R = Q;
        Assert::IsTrue(Equals(R.m_X, 1));
        Assert::IsTrue(Equals(R.m_Y, 2));
        Assert::IsTrue(Equals(R.m_Z, 3));
    }

    TEST_METHOD(TripleOperators)
    {
        Triple A(1, 10, 100);
        Triple B(A);
        Assert::IsTrue( A == B );
        Assert::IsFalse( A != B );

        Triple C( A+B );
        Assert::IsTrue( C == Triple(2, 20, 200) );

        B = C-A;
        Assert::IsTrue( B == Triple(1, 10, 100) );
    }

};

}