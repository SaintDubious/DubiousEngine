#include <Ac3d_file_reader.h>
#include <File_path.h>
#include <Point.h>
#include <Vector_math.h>
#include <Timer.h>
#include <Arena.h>
#include <Physics_object.h>
#include <Physics_model.h>

#include <memory>
#include <iostream>

using namespace Dubious;

// 03/31/2017 - BLOCK_COUNT = 3000, ITERATIONS = 100, Collision::SINGLE_THREADED, Constraint::SINGLE_THREADED - 7050ms
// 05/07/2017 - BLOCK_COUNT = 3000, ITERATIONS = 100, Collision::SINGLE_THREADED, Constraint::MULTI_THREADED - 5000ms
// 05/07/2017 - BLOCK_COUNT = 3000, ITERATIONS = 100, Collision::MULTI_THREADED,  Constraint::MULTI_THREADED - 2770ms

int
main(int argc, char** argv)
{
    try {
        std::cout << "Starting test\n";

        const auto                                BLOCK_COUNT = 3000;
        const auto                                ITERATIONS  = 100;
        Utility::Timer                            frame_timer;
        Physics::Arena::Collision_solver_settings collision_solver_settings;
        collision_solver_settings.strategy =
            Physics::Arena::Collision_solver_settings::Strategy::SINGLE_THREADED;
        collision_solver_settings.mt_collisions_work_group_size = 500;
        collision_solver_settings.cl_collisions_per_thread      = 10000;
        Physics::Arena::Constraint_solver_settings constraint_solver_settings;
        constraint_solver_settings.iterations = ITERATIONS;
        Physics::Arena arena(
            Physics::Arena::Settings(collision_solver_settings, constraint_solver_settings));
        std::vector<std::shared_ptr<Physics::Physics_object>> physics_objects;

        auto floor_file  = Utility::Ac3d_file_reader::test_cube(20.0f, 0.5f, 20.0f);
        auto floor_model = std::make_shared<Physics::Physics_model>(*floor_file);

        auto cube_file  = Utility::Ac3d_file_reader::test_cube(0.5f, 0.5f, 0.5f);
        auto cube_model = std::make_shared<Physics::Physics_model>(*cube_file);

        physics_objects.push_back(std::make_shared<Physics::Physics_object>(
            floor_model, Physics::Physics_object::STATIONARY));
        physics_objects.back()->coordinate_space().translate(Math::Vector(0, -0.5f, 0));
        arena.push_back(physics_objects.back());
        for (int i = 0; i < BLOCK_COUNT; ++i) {
            physics_objects.push_back(std::make_shared<Physics::Physics_object>(cube_model, 1.0f));
            physics_objects.back()->coordinate_space().translate(Math::Vector(0, i + 0.5f, 0));
            arena.push_back(physics_objects.back());
        }

        const auto ONE_SIXTIETH = 1.0f / 60.0f;
        frame_timer.start();
        for (int i = 0; i < 120; ++i) {
            for (auto& o : physics_objects) {
                o->force() = Math::Vector(0, -10.0f, 0);
            }
            arena.run_physics(ONE_SIXTIETH);
        }
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
