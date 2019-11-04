#include "CppUnitTest.h"

#include <Physics_model.h>
#include <Physics_object.h>
#include <Arena.h>
#include <Coordinate_space.h>
#include <Ac3d_file_reader.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Dubious::Physics;
using namespace Dubious::Math;
using namespace Dubious::Utility;

namespace Physics_test {

class Arena_test : public ::Microsoft::VisualStudio::CppUnitTestFramework::TestClass<Arena_test> {
public:
    TEST_METHOD(arena_linear_motion)
    {
        auto model_file = Ac3d_file_reader::test_cube(1.0f, 1.0f, 1.0f);
        auto model      = std::make_shared<Physics_model>(*model_file);
        auto a          = std::make_shared<Physics_object>(model, 10.0f);

        Arena arena((Arena::Settings()));

        a->force() = Vector(100, 0, 0);
        arena.push_back(a);
        arena.run_physics(1.0f);

        // bit of rounding error, but basically 10 m/s
        Assert::IsTrue(equals(a->velocity().length(), 9.99996090f));
    }

    TEST_METHOD(arena_angular_motion)
    {
        auto model_file = Ac3d_file_reader::test_cube(1.0f, 1.0f, 1.0f);
        auto model      = std::make_shared<Physics_model>(*model_file);
        auto a          = std::make_shared<Physics_object>(model, 10.0f);

        Arena arena((Arena::Settings()));

        // How did I arrive at this magic number?
        // I want the torque to accelerate this object to 90 deg/sec.
        // Remember that my moment of inertia is based on a sphere:
        // Im = 2/5 * m * r * r;
        // r = 1.73205078 for a cube of side 1
        // Im = 12.0f
        a->torque() = Vector(0, 18.849555921538759430775860299677f, 0);
        arena.push_back(a);
        arena.run_physics(1.0f);

        // bit of rounding error, but basically 90 degress/second
        Assert::IsTrue(equals(a->angular_velocity().length(), 1.57078958f));
        Assert::IsTrue(equals(a->angular_velocity().y(), 1.57078958f));
    }

    TEST_METHOD(stack_one_cube)
    {
        auto model_file = Ac3d_file_reader::test_cube(0.5f, 0.5f, 0.5f);
        auto model      = std::make_shared<Physics_model>(*model_file);
        auto floor      = std::make_shared<Physics_object>(model, Physics_object::STATIONARY);
        auto a          = std::make_shared<Physics_object>(model, 10.0f);

        floor->coordinate_space().translate(Vector(0, -0.5f, 0));
        a->coordinate_space().translate(Vector(0, 0.49f, 0));
        a->coordinate_space().rotate(Unit_quaternion(Unit_vector(0, 1.0f, 0), to_radians(90.0f)));

        Arena::Collision_solver_settings  collision;
        Arena::Constraint_solver_settings constraint;

        Arena arena((Arena::Settings(collision, constraint)));

        arena.push_back(floor);
        arena.push_back(a);
        a->force() = Vector(0, -9.8, 0);

        arena.run_physics(constraint.step_size + 0.000001f);
    }
};
}  // namespace Physics_test
