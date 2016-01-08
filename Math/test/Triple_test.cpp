#include "CppUnitTest.h"

#include <Triple.h>
#include <Utils.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Dubious::Math;

namespace Math_test {

TEST_CLASS(Triple_test) {
public:

    TEST_METHOD(triple_construction)
    {
        Triple p(1, 2, 3);
        Assert::IsTrue(equals(p.m_x, 1));
        Assert::IsTrue(equals(p.m_y, 2));
        Assert::IsTrue(equals(p.m_z, 3));

        Triple q(p);
        Assert::IsTrue(equals(q.m_x, 1));
        Assert::IsTrue(equals(q.m_y, 2));
        Assert::IsTrue(equals(q.m_z, 3));

        Triple r;
        Assert::IsTrue(equals(r.m_x, 0));
        Assert::IsTrue(equals(r.m_y, 0));
        Assert::IsTrue(equals(r.m_z, 0));

        r = q;
        Assert::IsTrue(equals(r.m_x, 1));
        Assert::IsTrue(equals(r.m_y, 2));
        Assert::IsTrue(equals(r.m_z, 3));
    }

    TEST_METHOD(triple_operators)
    {
        Triple a(1, 10, 100);
        Triple b(a);
        Assert::IsTrue( a == b );
        Assert::IsFalse( a != b );

        Triple c( a+b );
        Assert::IsTrue( c == Triple(2, 20, 200) );

        b = c-a;
        Assert::IsTrue( b == Triple(1, 10, 100) );
    }

};

}