#include <Ac3d_file_reader.h>
#include <File_path.h>
#include <Point.h>
#include <Vector_math.h>
#include <Timer.h>
#include <Arena.h>
#include <Physics_object.h>
#include <Physics_model.h>
#include <Unit_quaternion.h>

#include <memory>
#include <iostream>

using namespace Dubious;

// Result Log - sparse case
// 10/17/2016 - GROUP_COUNT=5000 - 22,800 ms - Starting point
// 11/11/2016 - GROUP_COUNT=5000 -  2,400 ms - optimized sparse case
// 11/23/2016 - GROUP_COUNT=5000 -  2,600 ms - with threading
// 02/20/2017 - GROUP_COUNT=5000 -    160 ms - opencl - broad phase
// 02/21/2017 - GROUP_COUNT=5000 -    120 ms - opencl - pre-create buffers
// 03/16/2017 - GROUP_COUNT=5000 -    765 ms - single-threaded - removed shared_ptr
// 03/16/2017 - GROUP_COUNT=5000 -     55 ms - multi-threaded - removed shared_ptr
// 03/16/2017 - GROUP_COUNT=5000 -    115 ms - opencl - removed shared_ptr
// 04/27/2017 - GROUP_COUNT=5000 -    230 ms - single-threaded - VS2017
// 04/27/2017 - GROUP_COUNT=5000 -     96 ms - opencl - cleanup, smaller collisions_per_thread, and VS2017

// Result Log - dense case
// 11/11/2016 - GROUP_COUNT=1000 - 12,800 ms - starting dense
// 11/23/2016 - GROUP_COUNT=1000 -  4,800 ms - with threading
// 03/19/2017 - GROUP_COUNT=1000 -  8,100 ms - single threaded - optimized
// 03/24/2017 - GROUP_COUNT=1000 -  6,100 ms - single threaded - optimized Minkowski_polytope
// 05/07/2017 - GROUP_COUNT=1000 -  3,100 ms - multi-threaded
// 05/07/2017 - GROUP_COUNT=1000 -  2,900 ms - OpenCL
// 07/03/2017 - GROUP_COUNT=1000 -  7,300 ms - proper quaternions

void
reset_scene(const int                                              group_count,
            std::vector<std::shared_ptr<Physics::Physics_object>>& physics_objects)
{
    const float DENSE  = 0.001f;
    const float SPARSE = 2.0f;
    for (int i = 0; i < group_count; ++i) {
        physics_objects[i * 2]->coordinate_space().position() = Math::Point(i * DENSE, 0, 0);
        physics_objects[i * 2]->coordinate_space().rotation() = Math::Unit_quaternion();
        physics_objects[i * 2]->velocity()                    = Math::Vector(0, 0.2f, 0);
        physics_objects[i * 2]->force()                       = Math::Vector(0, 1.0f, 0);
        physics_objects[i * 2]->angular_velocity()            = Math::Vector(0, 0, 0.2f);
        physics_objects[i * 2]->torque()                      = Math::Vector(0, 0, 0.2f);

        physics_objects[i * 2 + 1]->coordinate_space().position() = Math::Point(i * DENSE, 1.0f, 0);
        physics_objects[i * 2 + 1]->coordinate_space().rotation() = Math::Unit_quaternion();
        physics_objects[i * 2 + 1]->velocity()                    = Math::Vector(0, -0.2f, 0);
        physics_objects[i * 2 + 1]->force()                       = Math::Vector(0, -1.0f, 0);
        physics_objects[i * 2 + 1]->angular_velocity()            = Math::Vector(0, 0, -0.2f);
        physics_objects[i * 2 + 1]->torque()                      = Math::Vector(0, 0, -0.2f);
    }
}

int
main(int argc, char** argv)
{
    try {
        std::cout << "Starting test\n";

        const int                                 GROUP_COUNT = 1000;
        Utility::Timer                            frame_timer;
        Physics::Arena::Collision_solver_settings collision_solver_settings;
        collision_solver_settings.strategy =
            Physics::Arena::Collision_solver_settings::Strategy::SINGLE_THREADED;
        collision_solver_settings.mt_collisions_work_group_size = 500;
        collision_solver_settings.cl_collisions_per_thread      = 10000;
        Physics::Arena::Constraint_solver_settings constraint_solver_settings;
        constraint_solver_settings.iterations = 1;
        Physics::Arena arena(
            Physics::Arena::Settings(collision_solver_settings, constraint_solver_settings));

        std::vector<std::shared_ptr<Physics::Physics_object>> physics_objects;
        auto cube_file     = Utility::Ac3d_file_reader::test_cube(0.5f, 0.5f, 1.5f);
        auto physics_model = std::make_shared<Physics::Physics_model>(*cube_file);

        for (int i = 0; i < GROUP_COUNT; ++i) {
            physics_objects.push_back(
                std::make_shared<Physics::Physics_object>(physics_model, 1.0f));
            arena.push_back(physics_objects.back());
            physics_objects.push_back(
                std::make_shared<Physics::Physics_object>(physics_model, 1.0f));
            arena.push_back(physics_objects.back());
        }
        reset_scene(GROUP_COUNT, physics_objects);

        frame_timer.start();
        arena.run_physics(1.1f / 60.0f);
        std::cout << "elapsed: " << frame_timer.elapsed() << "\n";

        std::cout << "Ending Normally\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cout << "Caught top level exception: " << e.what() << "\n";
    }
    catch (...) {
        std::cout << "Caught top level exception\n";
    }
    return -1;
}
