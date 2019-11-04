#include "CppUnitTest.h"

#include <Physics_model.h>
#include <Physics_object.h>
#include <Collision_solver.h>
#include <Constraint_solver.h>
#include <Triple.h>
#include <Coordinate_space.h>
#include <Ac3d_file_reader.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Dubious::Physics;
using namespace Dubious::Math;
using namespace Dubious::Utility;

namespace Physics_test {

class Constraint_solver_test
    : public ::Microsoft::VisualStudio::CppUnitTestFramework::TestClass<Constraint_solver_test> {
public:
    TEST_METHOD(constraint_solver_two_cubes_one_point)
    {
        // Two cubes are placed with a 0.1 overlap on the Y axis. The
        // contact point is found to be right in the middle of the
        // cubes (0, 0.5, 0).
        // The expectation is that the cubes both get the same repulsive
        // velocity and no spin
        //
        auto model_file    = Ac3d_file_reader::test_cube(0.5f, 0.5f, 0.5f);
        auto physics_model = std::make_shared<Physics_model>(*model_file);

        // at (0,0,0)
        auto cube1 = std::make_shared<Physics_object>(physics_model, 1.0f);

        // at (0, 0.9, 0)
        auto cube2 = std::make_shared<Physics_object>(physics_model, 1.0f);
        cube2->coordinate_space().translate(Vector(0, 0.9f, 0));

        std::vector<Contact_manifold::Contact> contacts;
        Contact_manifold::Contact              c;
        c.contact_point_a   = Point(0, 0.5f, 0);
        c.local_point_a     = Local_point(0, 0.5f, 0);
        c.contact_point_b   = Point(0, 0.4f, 0);
        c.local_point_b     = Local_point(0, -0.5f, 0);
        c.normal            = Unit_vector(0, 1, 0);
        c.tangent1          = Unit_vector(0, 0, -1);
        c.tangent2          = Unit_vector(-1, 0, 0);
        c.penetration_depth = 0.1f;
        c.normal_impulse    = 0.0937500149f;
        contacts.push_back(c);

        Contact_manifold manifold(*cube1, *cube2, 0.05f, 0.05f);
        manifold.insert(contacts);
        Constraint_solver constraint_solver(0.016f, 0.03f, 0.5f, 0.05f);
        constraint_solver.solve(manifold);

        manifold.object_a().velocity() += manifold.a_delta_velocity();
        manifold.object_a().angular_velocity() += manifold.a_delta_angular_velocity();
        manifold.object_b().velocity() += manifold.b_delta_velocity();
        manifold.object_b().angular_velocity() += manifold.b_delta_angular_velocity();

        Assert::IsTrue(cube1->velocity() == -cube2->velocity());
        Assert::IsTrue(cube1->angular_velocity() == Vector());
        Assert::IsTrue(cube2->angular_velocity() == Vector());
    }

    TEST_METHOD(constraint_solver_two_cubes_two_points)
    {
        // Two cubes are placed with a 0.1 overlap on the Y axis. The
        // contact points are found to be on 2 corners of each cube.
        // The expectation is that the cubes both get the same repulsive
        // velocity and no spin
        //
        auto model_file    = Ac3d_file_reader::test_cube(0.5f, 0.5f, 0.5f);
        auto physics_model = std::make_shared<Physics_model>(*model_file);

        // at (0,0,0)
        auto cube1 = std::make_shared<Physics_object>(physics_model, 1.0f);

        // at (0, 0.9, 0)
        auto cube2 = std::make_shared<Physics_object>(physics_model, 1.0f);
        cube2->coordinate_space().translate(Vector(0, 0.9f, 0));

        std::vector<Contact_manifold::Contact> contacts;
        Contact_manifold::Contact              c;
        c.contact_point_a   = Point(0.5f, 0.5f, 0.5f);
        c.local_point_a     = Local_point(0.5f, 0.5f, 0.5f);
        c.contact_point_b   = Point(0.5f, 0.4f, 0.5f);
        c.local_point_b     = Local_point(0.5f, -0.5f, 0.5f);
        c.normal            = Unit_vector(0, 1, 0);
        c.tangent1          = Unit_vector(0, 0, -1);
        c.tangent2          = Unit_vector(-1, 0, 0);
        c.penetration_depth = 0.1f;
        c.normal_impulse    = 0.0937500149f;
        contacts.push_back(c);
        c.contact_point_a   = Point(-0.5f, 0.5f, -0.5f);
        c.local_point_a     = Local_point(-0.5f, 0.5f, -0.5f);
        c.contact_point_b   = Point(-0.5f, 0.4f, -0.5f);
        c.local_point_b     = Local_point(-0.5f, -0.5f, -0.5f);
        c.normal            = Unit_vector(0, 1, 0);
        c.tangent1          = Unit_vector(0, 0, -1);
        c.tangent2          = Unit_vector(-1, 0, 0);
        c.penetration_depth = 0.1f;
        c.normal_impulse    = 0.0937500149f;
        contacts.push_back(c);

        Contact_manifold manifold(*cube1, *cube2, 0.05f, 0.05f);
        manifold.insert(contacts);
        Constraint_solver constraint_solver(0.016f, 0.03f, 0.5f, 0.05f);
        constraint_solver.solve(manifold);

        manifold.object_a().velocity() += manifold.a_delta_velocity();
        manifold.object_a().angular_velocity() += manifold.a_delta_angular_velocity();
        manifold.object_b().velocity() += manifold.b_delta_velocity();
        manifold.object_b().angular_velocity() += manifold.b_delta_angular_velocity();

        Assert::IsTrue(cube1->velocity() == -cube2->velocity());
        Assert::IsTrue(cube1->angular_velocity() == Vector());
        Assert::IsTrue(cube2->angular_velocity() == Vector());
    }

    TEST_METHOD(constraint_solver_two_cubes_one_offset_point)
    {
        // Two cubes are placed with a 0.1 overlap on the Y axis. The
        // contact point is found to be offset in the cubes (0.5, 0.5, 0).
        // The expectation is that the cubes both get the same repulsive
        // velocity and no spin
        //
        auto model_file    = Ac3d_file_reader::test_cube(0.5f, 0.5f, 0.5f);
        auto physics_model = std::make_shared<Physics_model>(*model_file);

        // at (0,0,0)
        auto cube1 = std::make_shared<Physics_object>(physics_model, 1.0f);

        // at (0, 0.9, 0)
        auto cube2 = std::make_shared<Physics_object>(physics_model, 1.0f);
        cube2->coordinate_space().translate(Vector(0, 0.9f, 0));

        std::vector<Contact_manifold::Contact> contacts;
        Contact_manifold::Contact              c;
        c.contact_point_a   = Point(0.5f, 0.5f, 0);
        c.local_point_a     = Local_point(0.5f, 0.5f, 0);
        c.contact_point_b   = Point(0.5f, 0.4f, 0);
        c.local_point_b     = Local_point(0.5f, -0.5f, 0);
        c.normal            = Unit_vector(0, 1, 0);
        c.tangent1          = Unit_vector(0, 0, -1);
        c.tangent2          = Unit_vector(-1, 0, 0);
        c.penetration_depth = 0.1f;
        c.normal_impulse    = 0.0937500149f;
        contacts.push_back(c);

        Contact_manifold manifold(*cube1, *cube2, 0.05f, 0.05f);
        manifold.insert(contacts);
        Constraint_solver constraint_solver(0.016f, 0.03f, 0.5f, 0.05f);
        constraint_solver.solve(manifold);

        manifold.object_a().velocity() += manifold.a_delta_velocity();
        manifold.object_a().angular_velocity() += manifold.a_delta_angular_velocity();
        manifold.object_b().velocity() += manifold.b_delta_velocity();
        manifold.object_b().angular_velocity() += manifold.b_delta_angular_velocity();

        Assert::IsTrue(cube1->velocity() == -cube2->velocity());
        Assert::IsTrue(cube1->angular_velocity() == -cube2->angular_velocity());
    }

    TEST_METHOD(constraint_solver_three_cubes_one_point)
    {
        // This test has three cubes with the ones on top and bottom both
        // overlapping the one in the middle by an equal amount. There's
        // one contact point in each pair, right in the middle.
        // The expectation is that the middle cube doesn't move at all
        // and that the top and bottom cube both move with an equal
        // velocity. None of the cubes should have any spin
        //
        auto model_file    = Ac3d_file_reader::test_cube(0.5f, 0.5f, 0.5f);
        auto physics_model = std::make_shared<Physics_model>(*model_file);

        // at (0,0,0)
        auto cube1 = std::make_shared<Physics_object>(physics_model, 1.0f);

        // at (0, 0.9, 0)
        auto cube2 = std::make_shared<Physics_object>(physics_model, 1.0f);
        cube2->coordinate_space().translate(Vector(0, 0.9f, 0));

        // at (0, -0.9, 0)
        auto cube3 = std::make_shared<Physics_object>(physics_model, 1.0f);
        cube3->coordinate_space().translate(Vector(0, -0.9f, 0));

        std::vector<Contact_manifold::Contact> contacts_1;
        {
            Contact_manifold::Contact c;
            c.contact_point_a   = Point(0, 0.5f, 0);
            c.local_point_a     = Local_point(0, 0.5f, 0);
            c.contact_point_b   = Point(0, 0.4f, 0);
            c.local_point_b     = Local_point(0, -0.5f, 0);
            c.normal            = Unit_vector(0, 1, 0);
            c.tangent1          = Unit_vector(0, 0, -1);
            c.tangent2          = Unit_vector(-1, 0, 0);
            c.penetration_depth = 0.1f;
            c.normal_impulse    = 0.0937500149f;
            contacts_1.push_back(c);
        }
        Contact_manifold manifold_1(*cube1, *cube2, 0.05f, 0.05f);
        manifold_1.insert(contacts_1);

        std::vector<Contact_manifold::Contact> contacts_2;
        {
            Contact_manifold::Contact c;
            c.contact_point_a   = Point(0, -0.5f, 0);
            c.local_point_a     = Local_point(0, -0.5f, 0);
            c.contact_point_b   = Point(0, -0.4f, 0);
            c.local_point_b     = Local_point(0, 0.5f, 0);
            c.normal            = Unit_vector(0, -1, 0);
            c.tangent1          = Unit_vector(0, 0, -1);
            c.tangent2          = Unit_vector(-1, 0, 0);
            c.penetration_depth = 0.1f;
            c.normal_impulse    = 0.0937500149f;
            contacts_2.push_back(c);
        }
        Contact_manifold manifold_2(*cube1, *cube3, 0.05f, 0.05f);
        manifold_2.insert(contacts_2);

        Constraint_solver constraint_solver(0.016f, 0.03f, 0.5f, 0.05f);
        constraint_solver.solve(manifold_1);
        constraint_solver.solve(manifold_2);

        manifold_1.object_a().velocity() += manifold_1.a_delta_velocity();
        manifold_1.object_a().angular_velocity() += manifold_1.a_delta_angular_velocity();
        manifold_1.object_b().velocity() += manifold_1.b_delta_velocity();
        manifold_1.object_b().angular_velocity() += manifold_1.b_delta_angular_velocity();

        manifold_2.object_a().velocity() += manifold_2.a_delta_velocity();
        manifold_2.object_a().angular_velocity() += manifold_2.a_delta_angular_velocity();
        manifold_2.object_b().velocity() += manifold_2.b_delta_velocity();
        manifold_2.object_b().angular_velocity() += manifold_2.b_delta_angular_velocity();

        Assert::IsTrue(cube1->velocity() == Vector());
        Assert::IsTrue(cube2->velocity() == -cube3->velocity());
        Assert::IsTrue(cube1->angular_velocity() == Vector());
        Assert::IsTrue(cube2->angular_velocity() == Vector());
        Assert::IsTrue(cube3->angular_velocity() == Vector());
    }
};
}  // namespace Physics_test
