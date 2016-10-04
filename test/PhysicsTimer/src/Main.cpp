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

//////////////////////////////////////////////////////////////
void reset_scene( const int group_count, std::vector<std::shared_ptr<Physics::Physics_object>>& physics_objects ) 
{
    for (int i=0; i<group_count; ++i) {
        physics_objects[i*2]->coordinate_space().position()     = Math::Point( i, 0, 0 );
        physics_objects[i*2]->coordinate_space().rotation()     = Math::Quaternion();
        physics_objects[i*2]->velocity()                        = Math::Vector( 0, 0.2f, 0 );
        physics_objects[i*2]->angular_velocity()                = Math::Vector( 0, 0, 0.2f );
        physics_objects[i*2+1]->coordinate_space().position()   = Math::Point( i, 1.0f, 0 );
        physics_objects[i*2+1]->coordinate_space().rotation()   = Math::Quaternion();
        physics_objects[i*2+1]->velocity()                      = Math::Vector( 0, -0.2f, 0 );
        physics_objects[i*2+1]->angular_velocity()              = Math::Vector( 0, 0, -0.2f );
    }
}

//////////////////////////////////////////////////////////////
int main( int argc, char** argv )
{
    try {
        std::cout << "Starting test\n";

        const int GROUP_COUNT = 1000;
        Utility::Timer                      frame_timer;
        Physics::Arena                      arena( 1.0f/60.0f );
        std::vector<std::shared_ptr<Physics::Physics_object>>   physics_objects;
        auto cube_file = Utility::Ac3d_file_reader::test_cube( 0.5f, 0.5f, 0.5f );
        auto physics_model = std::make_shared<Physics::Physics_model>( *cube_file );





        for (int i=0; i<GROUP_COUNT; ++i) {
            physics_objects.push_back( std::make_shared<Physics::Physics_object>( physics_model, 1.0f ) );
            arena.push_back( physics_objects.back() );
            physics_objects.push_back( std::make_shared<Physics::Physics_object>( physics_model, 1.0f ) );
            arena.push_back( physics_objects.back() );
        }
        reset_scene( GROUP_COUNT, physics_objects );



        frame_timer.start();
        arena.run_physics( 1.1f/60.0f );
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

