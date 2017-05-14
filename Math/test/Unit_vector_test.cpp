#include "CppUnitTest.h"

#include <Unit_vector.h>
#include <Vector.h>
#include <Utils.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Dubious::Math;

namespace Math_test {

class Unit_vector_test
    : public ::Microsoft::VisualStudio::CppUnitTestFramework::TestClass<Unit_vector_test> {
public:
    TEST_METHOD(unit_vector_construction)
    {
        Unit_vector junk;
        Assert::IsTrue(equals(junk.x(), 1));
        Assert::IsTrue(equals(junk.y(), 0));
        Assert::IsTrue(equals(junk.z(), 0));

        Unit_vector p(1, 2, 3);
        Assert::IsTrue(equals(p.x(), 0.267261237f));
        Assert::IsTrue(equals(p.y(), 0.534522474f));
        Assert::IsTrue(equals(p.z(), 0.801783681f));

        Unit_vector q(p);
        Assert::IsTrue(equals(q.x(), 0.267261237f));
        Assert::IsTrue(equals(q.y(), 0.534522474f));
        Assert::IsTrue(equals(q.z(), 0.801783681f));

        Vector      r(1, 2, 3);
        Unit_vector s(r);
        Assert::IsTrue(equals(s.x(), 0.267261237f));
        Assert::IsTrue(equals(s.y(), 0.534522474f));
        Assert::IsTrue(equals(s.z(), 0.801783681f));

        try {
            Unit_vector u(0, 0, 0);
            Assert::Fail(L"Empty Unit Vector did not throw std::runtime_error");
        }
        catch (const std::runtime_error&) {
        }
        catch (...) {
            Assert::Fail(L"Empty Unit Vector threw wrong exception type");
        }

        try {
            Vector      v;
            Unit_vector u(v);
            Assert::Fail(L"Unit Vector from empty Vector did not throw std::runtime_error");
        }
        catch (const std::runtime_error&) {
        }
        catch (...) {
            Assert::Fail(L"Unit Vector from empty Vector thre wrong exception type");
        }
    }

    TEST_METHOD(unit_vector_operators)
    {
        Unit_vector p(1, 2, 3);
        Unit_vector q(1, 1, 1);

        Assert::IsTrue(q != p);

        q = p;
        Assert::IsTrue(equals(q.x(), 0.267261237f));
        Assert::IsTrue(equals(q.y(), 0.534522474f));
        Assert::IsTrue(equals(q.z(), 0.801783681f));

        Assert::IsTrue(q == p);
    }
};

}  // namespace Math_test