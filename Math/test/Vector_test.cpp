#include "CppUnitTest.h"

#include <Vector.h>
#include <Unit_vector.h>
#include <Utils.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Dubious::Math;

namespace Math_test {

class Vector_test : public ::Microsoft::VisualStudio::CppUnitTestFramework::TestClass<Vector_test> {
public:
    TEST_METHOD(vector_construction)
    {
        Vector o;
        Assert::IsTrue(equals(o.x(), 0));
        Assert::IsTrue(equals(o.y(), 0));
        Assert::IsTrue(equals(o.z(), 0));

        Vector p(1, 2, 3);
        Assert::IsTrue(equals(p.x(), 1));
        Assert::IsTrue(equals(p.y(), 2));
        Assert::IsTrue(equals(p.z(), 3));

        Vector q(p);
        Assert::IsTrue(equals(q.x(), 1));
        Assert::IsTrue(equals(q.y(), 2));
        Assert::IsTrue(equals(q.z(), 3));

        Unit_vector u(1, 2, 3);
        Vector      v(u);
        Assert::IsTrue(equals(v.x(), 0.267261237f));
        Assert::IsTrue(equals(v.y(), 0.534522474f));
        Assert::IsTrue(equals(v.z(), 0.801783681f));
    }

    TEST_METHOD(vector_operators)
    {
        Vector p(1, 2, 3);
        Vector q;
        Assert::IsTrue(q != p);
        q = p;
        Assert::IsTrue(equals(q.x(), 1));
        Assert::IsTrue(equals(q.y(), 2));
        Assert::IsTrue(equals(q.z(), 3));
        Assert::IsTrue(q == p);

        Vector r(10, 20, 30);
        q = r + p;
        Assert::IsTrue(equals(q.x(), 11));
        Assert::IsTrue(equals(q.y(), 22));
        Assert::IsTrue(equals(q.z(), 33));

        q = r - p;
        Assert::IsTrue(equals(q.x(), 9));
        Assert::IsTrue(equals(q.y(), 18));
        Assert::IsTrue(equals(q.z(), 27));

        q = r * 10;
        Assert::IsTrue(equals(q.x(), 100));
        Assert::IsTrue(equals(q.y(), 200));
        Assert::IsTrue(equals(q.z(), 300));

        q = -10 * r;
        Assert::IsTrue(equals(q.x(), -100));
        Assert::IsTrue(equals(q.y(), -200));
        Assert::IsTrue(equals(q.z(), -300));

        q = -r;
        Assert::IsTrue(equals(q.x(), -10));
        Assert::IsTrue(equals(q.y(), -20));
        Assert::IsTrue(equals(q.z(), -30));

        q += r;
        Assert::IsTrue(equals(q.x(), 0));
        Assert::IsTrue(equals(q.y(), 0));
        Assert::IsTrue(equals(q.z(), 0));

        q -= r;
        Assert::IsTrue(equals(q.x(), -10));
        Assert::IsTrue(equals(q.y(), -20));
        Assert::IsTrue(equals(q.z(), -30));
    }

    TEST_METHOD(vector_length)
    {
        Vector x(1, 2, 3);
        Assert::IsTrue(equals(x.length_squared(), 14));
        Assert::IsTrue(equals(x.length(), sqrt(14.0f)));
    }
};

}  // namespace Math_test