#include "CppUnitTest.h"

#include <Point.h>
#include <Vector.h>
#include <Unit_vector.h>
#include <Utils.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Dubious::Math;

namespace Math_test {

class Point_test : public ::Microsoft::VisualStudio::CppUnitTestFramework::TestClass<Point_test> {
public:
    TEST_METHOD(point_construction)
    {
        Point o;
        Assert::IsTrue(equals(o.x(), 0));
        Assert::IsTrue(equals(o.y(), 0));
        Assert::IsTrue(equals(o.z(), 0));

        Point p(1, 2, 3);
        Assert::IsTrue(equals(p.x(), 1));
        Assert::IsTrue(equals(p.y(), 2));
        Assert::IsTrue(equals(p.z(), 3));

        Point q(p);
        Assert::IsTrue(equals(q.x(), 1));
        Assert::IsTrue(equals(q.y(), 2));
        Assert::IsTrue(equals(q.z(), 3));
    }

    TEST_METHOD(point_operators)
    {
        Point q(1, 2, 3);
        Point p;

        Assert::IsTrue(p != q);
        p = q;
        Assert::IsTrue(p == q);
        Assert::IsTrue(equals(p.x(), 1));
        Assert::IsTrue(equals(p.y(), 2));
        Assert::IsTrue(equals(p.z(), 3));

        Point  r(10, 20, 30);
        Vector result = r - q;
        Assert::IsTrue(equals(result.x(), 9));
        Assert::IsTrue(equals(result.y(), 18));
        Assert::IsTrue(equals(result.z(), 27));

        p = r + result;
        Assert::IsTrue(equals(p.x(), 19));
        Assert::IsTrue(equals(p.y(), 38));
        Assert::IsTrue(equals(p.z(), 57));

        p = r - result;
        Assert::IsTrue(equals(p.x(), 1));
        Assert::IsTrue(equals(p.y(), 2));
        Assert::IsTrue(equals(p.z(), 3));

        Unit_vector unit_result(result);
        p = r + unit_result;
        Assert::IsTrue(equals(p.x(), 10.2672615f));
        Assert::IsTrue(equals(p.y(), 20.5345230f));
        Assert::IsTrue(equals(p.z(), 30.8017845f));

        p = r - unit_result;
        Assert::IsTrue(equals(p.x(), 9.73273849f));
        Assert::IsTrue(equals(p.y(), 19.4654770f));
        Assert::IsTrue(equals(p.z(), 29.1982155f));
    }

};  // namespace Math_test
}  // namespace Math_test