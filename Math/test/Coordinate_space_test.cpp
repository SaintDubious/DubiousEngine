#include "CppUnitTest.h"

#include <Coordinate_space.h>
#include <Point.h>

#include <tuple>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace Dubious::Math;

namespace Math_test {

class Coordinate_space_test
    : public ::Microsoft::VisualStudio::CppUnitTestFramework::TestClass<Coordinate_space_test> {
public:
    TEST_METHOD(coordinate_space_construction)
    {
        Coordinate_space c;
        Assert::IsTrue(c.position() == Point(0, 0, 0));
    }

    TEST_METHOD(coordinate_space_operators)
    {
        Coordinate_space a, b;
        Assert::IsTrue(a == b);
        b.translate(Vector(1, 0, 0));
        Assert::IsFalse(a == b);
        b.translate(Vector(-1, 0, 0));
        Assert::IsTrue(a == b);
        b.rotate(Unit_quaternion(Unit_vector(0, 1, 0), to_radians(45)));
        Assert::IsFalse(a == b);
        b.translate(Vector(1, 0, 0));
        Assert::IsFalse(a == b);
    }

    TEST_METHOD(coordinate_space_translation)
    {
        Coordinate_space c;
        Assert::IsTrue(c.position() == Point(0, 0, 0));
        c.translate(Vector(1, 2, 3));
        Assert::IsTrue(c.position() == Point(1, 2, 3));
        c.translate(Vector(1, 2, 3));
        Assert::IsTrue(c.position() == Point(2, 4, 6));

        c.rotate(Unit_quaternion(Unit_vector(0, 1, 0), to_radians(90)));
        c.translate(Vector(1, 2, 3));
        Assert::IsTrue(c.position() == Point(3, 6, 9));
        c.translate(Local_vector(1, 2, 3));
        Assert::IsTrue(c.position() == Point(6, 8, 8));
    }

    TEST_METHOD(coordinate_space_rotation)
    {
        Coordinate_space c;
        Unit_vector      x, y, z;
        std::tie(x, y, z) = c.get_axes();
        Assert::IsTrue(x == Unit_vector(1, 0, 0));
        Assert::IsTrue(y == Unit_vector(0, 1, 0));
        Assert::IsTrue(z == Unit_vector(0, 0, 1));
        c.rotate(Unit_quaternion(Unit_vector(0, 1, 0), to_radians(90)));
        std::tie(x, y, z) = c.get_axes();
        Assert::IsTrue(x == Unit_vector(0, 0, -1));
        Assert::IsTrue(y == Unit_vector(0, 1, 0));
        Assert::IsTrue(z == Unit_vector(1, 0, 0));
        c.rotate(Unit_quaternion(Unit_vector(0, 0, 1), to_radians(90)));
        std::tie(x, y, z) = c.get_axes();
        Assert::IsTrue(x == Unit_vector(0, 0, -1));
        Assert::IsTrue(y == Unit_vector(-1, 0, 0));
        Assert::IsTrue(z == Unit_vector(0, 1, 0));

        c.rotate(Local_unit_quaternion(Local_unit_vector(0, 0, -1), to_radians(90)));
        std::tie(x, y, z) = c.get_axes();
        Assert::IsTrue(x == Unit_vector(1, 0, 0));
        Assert::IsTrue(y == Unit_vector(0, 0, -1));
        Assert::IsTrue(z == Unit_vector(0, 1, 0));
        c.rotate(Local_unit_quaternion(Local_unit_vector(0, 1, 0), to_radians(90)));
        std::tie(x, y, z) = c.get_axes();
        Assert::IsTrue(x == Unit_vector(0, -1, 0));
        Assert::IsTrue(y == Unit_vector(0, 0, -1));
        Assert::IsTrue(z == Unit_vector(1, 0, 0));
    }

    TEST_METHOD(coordinate_space_vectortransform)
    {
        Coordinate_space c;
        c.rotate(Unit_quaternion(Vector(0, 1, 0), to_radians(90)));
        Vector v = c.transform(Local_vector(1, 1, 1));
        Assert::IsTrue(v == Vector(1, 1, -1));
        Local_vector lv = c.transform(Vector(1, 1, 1));
        Assert::IsTrue(lv == Local_vector(-1, 1, 1));
    }

    TEST_METHOD(coordinate_space_pointtransform)
    {
        Coordinate_space c;
        c.rotate(Unit_quaternion(Vector(0, 1, 0), to_radians(90)));
        c.position() = Point(5, 5, 5);
        Point p      = c.transform(Local_point(1, 1, 1));
        Assert::IsTrue(p == Point(6, 6, 4));
        Local_point lp = c.transform(Point(1, 1, 1));
        Assert::IsTrue(lp == Local_point(4, -4, -4));
    }

    // For this test I am using Blender to perform random rotations
    // on a line segment. Then I'll enter in the values Blender shows
    // me and make sure my Coordinate_space gets the same values
    TEST_METHOD(coordinate_space_blender_rotations)
    {
        Coordinate_space c;
        Unit_vector      x, y, z;
        c.rotate(Unit_quaternion(Unit_vector(-0.561f, -0.089f, -0.823f), to_radians(138.465f)));
        std::tie(x, y, z) = c.get_axes();
        Assert::IsTrue(Unit_vector(0.748319924f, 0.500071585f, 0.435827762f) == z);
        c.rotate(Unit_quaternion(Unit_vector(0.492f, -0.543f, 0.68f), to_radians(114.473f)));
        std::tie(x, y, z) = c.get_axes();
        Assert::IsTrue(Unit_vector(-0.561430097f, -0.241100222f, 0.791622996f) == z);
    }
};
}  // namespace Math_test