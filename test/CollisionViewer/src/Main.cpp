#define SDL_MAIN_HANDLED
#include <Sdl_manager.h>
#include <Ac3d_file_reader.h>
#include <File_path.h>
#include <Point.h>
#include <Scene.h>
#include <Camera.h>
#include <Visible_model.h>
#include <Visible_object.h>
#include <Simple_object_renderer.h>
#include <Outlined_object_renderer.h>
#include <Shadow_renderer.h>
#include <Open_gl_context_store.h>
#include <Vector_math.h>
#include <Timer.h>
#include <Collision_solver.h>
#include <Physics_object.h>
#include <Physics_model.h>

#include <memory>
#include <iostream>

using namespace Dubious;

//////////////////////////////////////////////////////////////
const float LIGHT_HEIGHT = 50.0f;
const float PI = 3.1415926535f;
const int WIDTH=800;
const int HEIGHT=600;

//////////////////////////////////////////////////////////////
// Events
void on_quit();
void on_idle();
void on_mouse_motion( const Utility::Sdl_manager::Mouse_point& P );
void on_mouse_left_down( const Utility::Sdl_manager::Mouse_point& P );
void on_mouse_left_up( const Utility::Sdl_manager::Mouse_point& P );
void on_mouse_wheel( int Y );
void on_key_down( SDL_Keycode Key, unsigned short Mod );

//////////////////////////////////////////////////////////////
// Globals
Utility::Sdl_manager	            sdl;
std::unique_ptr<Renderer::Scene>    scene;
std::unique_ptr<Renderer::Camera>   camera;
Utility::Sdl_manager::Mouse_point   left_down_point;
bool					            left_button_down;
std::shared_ptr<Renderer::Simple_object_renderer> simple_renderer;

Physics::Collision_solver           solver;
std::shared_ptr<Renderer::Visible_object>  visible_object_1;
std::shared_ptr<Physics::Physics_object>   physics_object_1;
std::shared_ptr<Renderer::Visible_object>  visible_object_2;
std::shared_ptr<Physics::Physics_object>   physics_object_2;
std::shared_ptr<Renderer::Visible_object>  selected_visible_object;
std::shared_ptr<Physics::Physics_object>   selected_physics_object;

//////////////////////////////////////////////////////////////
int main( int argc, char** argv )
{
    try {
        std::cout << "Starting test\n";
        sdl.create_root_window( "Collision Viewer", WIDTH, HEIGHT, false );

        std::shared_ptr<Renderer::Visible_model> visible_model( new Renderer::Visible_model( Math::Triple( 1, 1, 1 ), false ) );
        std::shared_ptr<Physics::Physics_model> physics_model( new Physics::Physics_model( Math::Triple( 1, 1, 1 ) ) );

        visible_object_1 = std::shared_ptr<Renderer::Visible_object>( new Renderer::Visible_object(visible_model, visible_model) );
        visible_object_1->coordinate_space().translate( Math::Vector( 0, 0, 0 ) );
        physics_object_1 = std::shared_ptr<Physics::Physics_object>( new Physics::Physics_object( physics_model ) );
        physics_object_1->coordinate_space().translate( Math::Vector( 0, 0, 0 ) );

        visible_object_2 = std::shared_ptr<Renderer::Visible_object>( new Renderer::Visible_object(visible_model, visible_model) );
        visible_object_2->coordinate_space().translate( Math::Vector( 3, 0, 0 ) );
        physics_object_2 = std::shared_ptr<Physics::Physics_object>( new Physics::Physics_object( physics_model ) );
        physics_object_2->coordinate_space().translate( Math::Vector( 3, 0, 0 ) );

        selected_visible_object = visible_object_1;
        selected_physics_object = physics_object_1;

        std::shared_ptr<Renderer::Open_gl_context_store> context_store( new Renderer::Open_gl_context_store );
        scene = std::unique_ptr<Renderer::Scene>( new Renderer::Scene(context_store) );
        simple_renderer.reset( new Renderer::Simple_object_renderer( context_store ) );

        visible_object_1->renderer() = simple_renderer;
        visible_object_2->renderer() = simple_renderer;

        scene->scene_light().position = Math::Point( 20, 20, 0 );
        scene->scene_light().ambient = Renderer::Color( 0.2f, 0.2f, 0.2f, 1.0f );
        scene->scene_light().diffuse = Renderer::Color( 0.5f, 0.5f, 0.5f, 1.0f );
        scene->scene_light().specular = Renderer::Color( 0.1f, 0.1f, 0.1f, 1.0f );

        scene->add_object( visible_object_1 );
        scene->add_object( visible_object_2 );

        camera = std::unique_ptr<Renderer::Camera>( new Renderer::Camera( 0, 0, WIDTH,HEIGHT, 80.0f ) );
        camera->z_axis_offset() = 20;

        sdl.on_quit() = on_quit;
        sdl.on_idle() = on_idle;
        sdl.on_mouse_motion() = on_mouse_motion;
        sdl.on_mouse_left_down() = on_mouse_left_down;
        sdl.on_mouse_left_up() = on_mouse_left_up;
        sdl.on_mouse_wheel() = on_mouse_wheel;
        sdl.on_key_down() = on_key_down;

        std::cout << "Object 1 Selected\n";
        sdl.run();

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

//////////////////////////////////////////////////////////////
void on_quit()
{
}

//////////////////////////////////////////////////////////////
void on_idle()
{
//    Math::Quaternion Q( Math::Vector( 0, 1.0f, 0 ), Math::to_radians( 1.0f ) );
//    pObject->coordinate_space().rotate( Q );
    scene->render( *camera );

    // no reason to push the CPU to 100%
    SDL_Delay( 10 );
}

//////////////////////////////////////////////////////////////
void on_mouse_motion( const Utility::Sdl_manager::Mouse_point& p )
{
    if( left_button_down ){
        Utility::Sdl_manager::Mouse_point offset = std::make_pair( left_down_point.first - p.first, left_down_point.second - p.second );
        Math::Quaternion y_rotation( Math::Vector( 0, 1.0f, 0 ), Math::to_radians(static_cast<float>(offset.first)) ); 
        camera->coordinate_space().rotate( y_rotation );

        // Rotation around the local XAxis is limited to about 80 degrees in
        // either direction.
        Math::Vector max_y( 0, camera->z_axis_offset(), 0 );
        Math::Unit_vector z;
        std::tie( std::ignore, std::ignore, z) = camera->coordinate_space().get_axes();
        Math::Vector vz( z );
        vz = vz * camera->z_axis_offset();
        if (offset.second > 0) {
            if (Math::dot_product( max_y, vz ) > -(camera->z_axis_offset()*camera->z_axis_offset()) * 0.9f) {
                Math::Local_quaternion x_rotation( Math::Local_vector( 1.0f, 0, 0 ), Math::to_radians(static_cast<float>(offset.second)) );
                camera->coordinate_space().rotate( x_rotation );
            }
        }
        else if (offset.second < 0) {
            if (Math::dot_product( max_y, vz ) < (camera->z_axis_offset()*camera->z_axis_offset()) * 0.9f) {
                Math::Local_quaternion x_rotation( Math::Local_vector( 1.0f, 0, 0 ), Math::to_radians(static_cast<float>(offset.second)) );
                camera->coordinate_space().rotate( x_rotation );
            }
        }

        left_down_point = p;
    }
}

//////////////////////////////////////////////////////////////
void on_mouse_left_down( const Utility::Sdl_manager::Mouse_point& p )
{
    left_button_down = true;
    left_down_point = p;
}

//////////////////////////////////////////////////////////////
void on_mouse_left_up( const Utility::Sdl_manager::Mouse_point&  )
{
    left_button_down = false;
}

//////////////////////////////////////////////////////////////
void on_mouse_wheel( int y )
{
    int delta = y*-5;
    if( camera->z_axis_offset() + delta < 5 ) {
        return;
    }
    camera->z_axis_offset() += delta;
}

//////////////////////////////////////////////////////////////
void on_key_down( SDL_Keycode key, unsigned short mod )
{
    switch( key )
    {
    case SDLK_q:
        sdl.stop();
        break;
    case SDLK_1:
        selected_visible_object = visible_object_1;
        selected_physics_object = physics_object_1;
        std::cout << "Object 1 Selected\n";
        break;
    case SDLK_2:
        selected_visible_object = visible_object_2;
        selected_physics_object = physics_object_2;
        std::cout << "Object 2 Selected\n";
        break;
    case SDLK_t:
        {
            std::cout << "Object 1: " << physics_object_1->coordinate_space() << "\n"
                      << "Object 2: " << physics_object_2->coordinate_space() << "\n";
            bool Intersection = solver.intersection( *physics_object_1, *physics_object_2 );
            if (Intersection) {
                std::cout << "Objects intersect\n";
            }
            else {
                std::cout << "Objects do not intersect\n";
            }
        }
        break;
    case SDLK_w:
        {
            Math::Vector D( 0, 0, -1 );
            selected_visible_object->coordinate_space().translate( D );
            selected_physics_object->coordinate_space().translate( D );
        }
        break;
    case SDLK_a:
        {
            Math::Vector D( -1, 0, 0 );
            selected_visible_object->coordinate_space().translate( D );
            selected_physics_object->coordinate_space().translate( D );
        }
        break;
    case SDLK_s:
        {
            Math::Vector D( 0, 0, 1 );
            selected_visible_object->coordinate_space().translate( D );
            selected_physics_object->coordinate_space().translate( D );
        }
        break;
    case SDLK_d:
        {
            Math::Vector D( 1, 0, 0 );
            selected_visible_object->coordinate_space().translate( D );
            selected_physics_object->coordinate_space().translate( D );
        }
        break;
    case SDLK_z:
        {
            Math::Vector D( 0, 1, 0 );
            selected_visible_object->coordinate_space().translate( D );
            selected_physics_object->coordinate_space().translate( D );
        }
        break;
    case SDLK_x:
        {
            Math::Vector D( 0, -1, 0 );
            selected_visible_object->coordinate_space().translate( D );
            selected_physics_object->coordinate_space().translate( D );
        }
        break;
    case SDLK_r:
        {
            srand( static_cast<unsigned long>(time(NULL)) );
            Math::Quaternion Q( Math::Vector( static_cast<float>(rand())/static_cast<float>(RAND_MAX), 
                                              static_cast<float>(rand())/static_cast<float>(RAND_MAX), 
                                              static_cast<float>(rand())/static_cast<float>(RAND_MAX) ), 
                                              Math::to_radians(360.0f*static_cast<float>(rand())/static_cast<float>(RAND_MAX)) );
            selected_visible_object->coordinate_space().rotate( Q );
            selected_physics_object->coordinate_space().rotate( Q );
        }
        break;
    }
}

