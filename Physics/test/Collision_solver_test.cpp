#include "CppUnitTest.h"

#include <Physics_model.h>
#include <Physics_object.h>
#include <Collision_solver.h>
#include <Triple.h>
#include <Coordinate_space.h>
#include <Ac3d_file_reader.h>
#include <Unit_quaternion.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Dubious::Physics;
using namespace Dubious::Math;
using namespace Dubious::Utility;

namespace Physics_test {

class Collision_solver_test
    : public ::Microsoft::VisualStudio::CppUnitTestFramework::TestClass<Collision_solver_test> {
public:
    TEST_METHOD(collision_solver_intersection)
    {
        Collision_solver                       solver(false);
        std::vector<Contact_manifold::Contact> contacts;
        std::unique_ptr<const Ac3d_file> model_file = Ac3d_file_reader::test_cube(1.0f, 1.0f, 1.0f);

        std::shared_ptr<Physics_model> model = std::make_shared<Physics_model>(*model_file);
        Physics_object                 a(model, 1);
        Physics_object                 b(model, 1);

        b.coordinate_space().translate(Vector(3, 0, 0));
        Assert::IsTrue(solver.intersection(a, b, contacts) == false);
        // Touching... this is not a collision
        b.coordinate_space().translate(Vector(-1, 0, 0));
        Assert::IsTrue(solver.intersection(a, b, contacts) == false);
        // definite collision
        b.coordinate_space().translate(Vector(-1, 0, 0));
        Assert::IsTrue(solver.intersection(a, b, contacts) == true);

        // Add some rotation
        b.coordinate_space().rotate(Unit_quaternion(Vector(0, 1, 0), to_radians(30)));
        Assert::IsTrue(solver.intersection(a, b, contacts) == true);
        b.coordinate_space().rotate(Unit_quaternion(Vector(1, 0, 0), to_radians(30)));
        Assert::IsTrue(solver.intersection(a, b, contacts) == true);

        // Test that failed in CollisionViewer
        a.coordinate_space() = Coordinate_space(
            Point(1, 4, -1),
            Unit_quaternion(-0.191323161f, Vector(0.479090452f, 0.852346003f, 0.0858706832f)));
        b.coordinate_space() = Coordinate_space(
            Point(2, 2, -1),
            Unit_quaternion(-0.737145424f, Vector(-0.451694548f, 0.245187759f, 0.438716054f)));
        Assert::IsTrue(solver.intersection(a, b, contacts) == true);

        // This one triggers the escape from infinite loop in the collision solver.
        // Not sure if it should be a collision or not, but it's a good test
        a.coordinate_space() = Coordinate_space(
            Point(0.200000003f, 0, -0.400000006f),
            Unit_quaternion(0.874503672f, Vector(-0.214069009f, -0.331128120f, 0.283099681f)));
        b.coordinate_space() = Coordinate_space(
            Point(3, 0, 0), Unit_quaternion(-0.805550456f, Vector(0.0f, 0.592838526f, 0.0f)));
        Assert::IsTrue(solver.intersection(a, b, contacts) == false);

        // This one triggers a flat Minkowski Simplex, an attempt to construct a Unit Vector
        // of zero length, and an exception
        {
            const float OBJECT_WIDTH  = 0.008f;
            const float OBJECT_HEIGHT = 0.024f;
            const float OBJECT_DEPTH  = 0.12f;

            std::unique_ptr<const Ac3d_file> model_file2 = Ac3d_file_reader::test_cube(
                OBJECT_WIDTH / 2.0f, OBJECT_HEIGHT / 2.0f, OBJECT_DEPTH / 2.0f);
            std::shared_ptr<Physics_model> model2 = std::make_shared<Physics_model>(*model_file2);
            Physics_object                 a2(model2, 1);
            Physics_object                 b2(model2, 1);
            a2.coordinate_space() = Coordinate_space(
                Point(-2.23147072e-06f, 0.0572698712f, -0.0999976322f),
                Unit_quaternion(0.707087040f,
                                Vector(-6.64497275e-05f, 0.707126498f, 8.92839889e-05f)));
            b2.coordinate_space() = Coordinate_space(
                Point(0.0707066581f, 0.0812881067f, -0.0706997886f),
                Unit_quaternion(0.923927546f,
                                Vector(-0.000113081020f, 0.382567436f, 1.36329363e-05f)));
            Assert::IsTrue(solver.intersection(a2, b2, contacts) == false);
        }
    }

    TEST_METHOD(collision_solver_touching_contacts)
    {
        Collision_solver                       solver(false);
        std::vector<Contact_manifold::Contact> contacts;
        std::unique_ptr<const Ac3d_file> model_file = Ac3d_file_reader::test_cube(5.0f, 0.5f, 5.0f);

        std::shared_ptr<Physics_model> model = std::make_shared<Physics_model>(*model_file);
        Physics_object                 a(model, 1);
        Physics_object                 b(model, 1);

        a.coordinate_space() = Coordinate_space(Point(0, 0, -0.1f), Unit_quaternion(1, Vector()));
        b.coordinate_space() = Coordinate_space(Point(3, 0, 0), Unit_quaternion(1, Vector()));
        Assert::IsTrue(solver.intersection(a, b, contacts) == true);
    }
};
}  // namespace Physics_test