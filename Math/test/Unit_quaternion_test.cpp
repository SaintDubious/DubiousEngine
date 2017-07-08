#include "CppUnitTest.h"

#include <Unit_quaternion.h>
#include <Quaternion.h>
#include <Quaternion_math.h>
#include <Unit_vector.h>
#include <Utils.h>
#include <Triple.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Dubious::Math;

namespace Math_test {

class Unit_quaternion_test
    : public ::Microsoft::VisualStudio::CppUnitTestFramework::TestClass<Unit_quaternion_test> {
public:
    TEST_METHOD(unit_quaternion_construction)
    {
        Unit_quaternion q;
        Assert::IsTrue(equals(q.w(), 1));
        Assert::IsTrue(q.v() == Vector());
        Assert::IsTrue(equals(q.magnitude(), 1));

        Unit_quaternion r(Unit_vector(1, 0, 0), to_radians(60));
        Assert::IsTrue(equals(r.w(), cosf(to_radians(60) / 2.0f)));
        float HalfSin = sinf(to_radians(60) / 2.0f);
        Assert::IsTrue(r.v() == Vector(HalfSin, 0, 0));
        Assert::IsTrue(equals(r.magnitude(), 1));

        Unit_quaternion s(r);
        Assert::IsTrue(equals(s.w(), cosf(to_radians(60) / 2.0f)));
        Assert::IsTrue(s.v() == Vector(HalfSin, 0, 0));
        Assert::IsTrue(equals(s.magnitude(), 1));

        Quaternion      u(2.1f, Vector(1, 2, 3));
        Unit_quaternion t(u);
        float           mag = sqrt(2.1f * 2.1f + 1 + 4 + 9);
        Assert::IsTrue(equals(t.w(), 2.1f / mag));
        Assert::IsTrue(equals(t.v().x(), 1.0f / mag));
        Assert::IsTrue(equals(t.v().y(), 2.0f / mag));
        Assert::IsTrue(equals(t.v().z(), 3.0f / mag));
        Assert::IsTrue(equals(t.magnitude(), 1));
    }

    TEST_METHOD(unit_quaternion_operators)
    {
        Unit_quaternion r(Unit_vector(0, 1, 0), to_radians(60));
        Unit_quaternion q;
        Assert::IsTrue(equals(q.magnitude(), 1));
        Assert::IsTrue(equals(r.magnitude(), 1));

        Assert::IsTrue(q != r);
        q = r;
        Assert::IsTrue(q == r);
        Assert::IsTrue(equals(q.w(), cosf(to_radians(60) / 2.0f)));
        float half_sin = sinf(to_radians(60) / 2.0f);
        Assert::IsTrue(q.v() == Vector(0, half_sin, 0));

        Unit_quaternion s(Unit_vector(0, 1, 0), to_radians(20));
        Assert::IsTrue(equals(s.magnitude(), 1));
        Unit_quaternion t = s * r;
        Assert::IsTrue(equals(t.magnitude(), 1));
        Unit_quaternion u(Unit_vector(0, 1, 0), to_radians(80));
        Assert::IsTrue(t == u);

        s = Unit_quaternion(Unit_vector(0, 1, 0), to_radians(10));
        t = Unit_quaternion(Unit_vector(0, 1, 0), to_radians(10));
        for (int i = 2; i < 18; ++i) {
            s = t * s;
            u = Unit_quaternion(Unit_vector(0, 1, 0), to_radians(i * 10.0f));
            Assert::IsTrue(equals(u.magnitude(), 1));
            Assert::IsTrue(s == u);
            Unit_quaternion conjugate = s.conjugate();
            Assert::IsTrue(equals(conjugate.magnitude(), 1));
            Assert::IsTrue(conjugate.w() == s.w());
            Assert::IsTrue(conjugate.v().x() == -s.v().x());
            Assert::IsTrue(conjugate.v().y() == -s.v().y());
            Assert::IsTrue(conjugate.v().z() == -s.v().z());
        }
    }

    TEST_METHOD(unit_quaternion_length)
    {
        // My understanding of Quaternion construction from Axis and
        // angle is that it results in a unit quaternion. This just
        // checks to see if that holds true for a bunch of samples
        for (int i = -10; i < 50; ++i) {
            for (int j = -360; j <= 360; j += 10) {
                Unit_quaternion r(Unit_vector(static_cast<float>(25 - i), static_cast<float>(i),
                                              static_cast<float>(i - 10)),
                                  to_radians(static_cast<float>(j)));
                Assert::IsTrue(equals(r.magnitude(), 1));
            }
        }
    }

    TEST_METHOD(unit_quaternion_matrix)
    {
        float           matrix[16];
        Unit_quaternion a;
        a.get_matrix(matrix);

        using namespace Dubious::Math::Matrix_index;

        Assert::IsTrue(equals(matrix[_11], 1));
        Assert::IsTrue(equals(matrix[_22], 1));
        Assert::IsTrue(equals(matrix[_33], 1));

        Unit_quaternion b(Unit_vector(0, 1, 0), to_radians(30));
        b.get_matrix(matrix);
        // X Axis
        Assert::IsTrue(equals(matrix[_11], cosf(to_radians(30))));
        Assert::IsTrue(equals(matrix[_21], 0));
        Assert::IsTrue(equals(matrix[_31], -sinf(to_radians(30))));
        // Y Axis
        Assert::IsTrue(equals(matrix[_12], 0));
        Assert::IsTrue(equals(matrix[_22], 1));
        Assert::IsTrue(equals(matrix[_32], 0));
        // Z Axis
        Assert::IsTrue(equals(matrix[_13], sinf(to_radians(30))));
        Assert::IsTrue(equals(matrix[_23], 0));
        Assert::IsTrue(equals(matrix[_33], cosf(to_radians(30))));
    }

    TEST_METHOD(unit_quaternion_axis_angle)
    {
        Unit_vector     axis(0.5f, 1.7f, -1.1f);
        float           angle = 1.2345f;
        Unit_quaternion a(axis, angle);
        Unit_vector     test_axis;
        float           test_angle;
        std::tie(test_axis, test_angle) = a.get_axis_angle();

        Assert::IsTrue(axis == test_axis);
        Assert::IsTrue(angle == test_angle);
    }

    TEST_METHOD(unit_quaternion_slerp)
    {
        Unit_quaternion a;
        Unit_quaternion b(Unit_vector(0, 0, 1.0f), to_radians(90.0f));
        Unit_vector     axis;
        float           angle;

        Unit_quaternion c     = slerp(a, b, 0.5f);
        std::tie(axis, angle) = c.get_axis_angle();
        Assert::IsTrue(equals(angle, to_radians(45.0f)));
        Assert::IsTrue(axis == Unit_vector(0, 0, 1.0f));

        c                     = slerp(a, b, 0.333333333f);
        std::tie(axis, angle) = c.get_axis_angle();
        Assert::IsTrue(equals(angle, to_radians(30.0f)));
        Assert::IsTrue(axis == Unit_vector(0, 0, 1.0f));

        c                     = slerp(a, b, 0.666666666f);
        std::tie(axis, angle) = c.get_axis_angle();
        Assert::IsTrue(equals(angle, to_radians(60.0f)));
        Assert::IsTrue(axis == Unit_vector(0, 0, 1.0f));

        // test small angles
        b                     = Unit_quaternion(Unit_vector(0, 0, 1.0f), to_radians(1.0f));
        c                     = slerp(a, b, 0.5f);
        std::tie(axis, angle) = c.get_axis_angle();
        // linear interpolation is not accurate, but close. In a perfect world
        // this would be to_radians(0.5f)
        Assert::IsTrue(equals(angle, 0.00870730914f));
        Assert::IsTrue(axis == Unit_vector(0, 0, 1.0f));

        // test long way around
        b                     = Unit_quaternion(Unit_vector(0, 0, 1.0f), to_radians(190.0f));
        c                     = slerp(a, b, 0.5f);
        std::tie(axis, angle) = c.get_axis_angle();
        float test            = to_radians(95.0f);
        float test1           = to_radians(275.0f);
        Assert::IsTrue(equals(angle, to_radians(95.0f)));
        Assert::IsTrue(axis == Unit_vector(0, 0, 1.0f));
    }
};

}  // namespace Math_test
