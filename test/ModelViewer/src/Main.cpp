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

#include <memory>
#include <iostream>

using namespace Dubious;

const float LIGHT_HEIGHT = 50.0f;
const float PI = 3.1415926535f;
const int WIDTH=800;
const int HEIGHT=600;

// Events
void on_quit();
void on_idle();
void on_mouse_motion( const Utility::Sdl_manager::Mouse_point& P );
void on_mouse_left_down( const Utility::Sdl_manager::Mouse_point& P );
void on_mouse_left_up( const Utility::Sdl_manager::Mouse_point& P );
void on_mouse_wheel( int Y );
void on_key_down( SDL_Keycode Key, unsigned short Mod );

// Globals
Utility::Sdl_manager	            sdl;
std::unique_ptr<Renderer::Scene>    scene;
std::unique_ptr<Renderer::Camera>   camera;
Utility::Sdl_manager::Mouse_point   left_down_point;
bool					            left_button_down;
std::shared_ptr<Renderer::Visible_object> object;
std::shared_ptr<Renderer::Simple_object_renderer> simple_renderer;
std::shared_ptr<Renderer::Outlined_object_renderer> outlined_renderer;
Utility::Timer                      frame_rate_timer;
int                                 frame_count;

int main( int argc, char** argv )
{
    if( argc != 3 ){
        std::cout << "Usage: ModelViewer ModelFile.ac ShadowModel.ac\n";
        return 0;
    }
    std::string visual_model_file = argv[1];
    std::string shadow_model_file = argv[2];

    try {
        std::cout << "Starting test\n";
        sdl.create_root_window( "Model Viewer", WIDTH, HEIGHT, false );

        std::unique_ptr<const Utility::Ac3d_file> floor_file = Utility::Ac3d_file_reader::read_file( Utility::File_path( "Floor.ac" ) );
        std::unique_ptr<const Utility::Ac3d_file> model_file = Utility::Ac3d_file_reader::read_file( Utility::File_path( visual_model_file ) );
        std::unique_ptr<const Utility::Ac3d_file> shadow_file = Utility::Ac3d_file_reader::read_file( Utility::File_path( shadow_model_file ) );

        std::shared_ptr<Renderer::Visible_model> floor_model = std::make_shared<Renderer::Visible_model>( *floor_file, false );
        std::shared_ptr<Renderer::Visible_model> model = std::make_shared<Renderer::Visible_model>( *model_file, false );
        std::shared_ptr<Renderer::Visible_model> shadow_model = std::make_shared<Renderer::Visible_model>( *shadow_file, true );

        std::shared_ptr<Renderer::Visible_object> floor_object = std::make_shared<Renderer::Visible_object>(floor_model, std::shared_ptr<Renderer::Visible_model>());
        object = std::make_shared<Renderer::Visible_object>( model, shadow_model);

        floor_object->coordinate_space().translate( Math::Vector( 0, -5, 0 ) );
        object->coordinate_space().translate( Math::Vector( 0, 0, 0 ) );

        std::shared_ptr<Renderer::Open_gl_context_store> context_store = std::make_shared<Renderer::Open_gl_context_store>();
        scene = std::make_unique<Renderer::Scene>( context_store );
        simple_renderer.reset( new Renderer::Simple_object_renderer( context_store ) );
        outlined_renderer.reset( new Renderer::Outlined_object_renderer( context_store ) );
        std::shared_ptr<Renderer::Shadow_renderer> shadow_renderer = std::make_shared<Renderer::Shadow_renderer>();
        floor_object->renderer() = simple_renderer;
        object->renderer() = outlined_renderer;
        object->shadow_renderer() = shadow_renderer;

        scene->scene_light().position = Math::Point( 20, 20, 0 );
        scene->scene_light().ambient = Renderer::Color( 0.2f, 0.2f, 0.2f, 1.0f );
        scene->scene_light().diffuse = Renderer::Color( 0.5f, 0.5f, 0.5f, 1.0f );
        scene->scene_light().specular = Renderer::Color( 0.1f, 0.1f, 0.1f, 1.0f );
        scene->add_object( floor_object );
        scene->add_object( object );

        camera = std::make_unique<Renderer::Camera>( 0, 0, WIDTH,HEIGHT, 80.0f );
        camera->z_axis_offset() = 20;
        
        frame_rate_timer.start();
        frame_count = 0;
        sdl.on_quit() = on_quit;
        sdl.on_idle() = on_idle;
        sdl.on_mouse_motion() = on_mouse_motion;
        sdl.on_mouse_left_down() = on_mouse_left_down;
        sdl.on_mouse_left_up() = on_mouse_left_up;
        sdl.on_mouse_wheel() = on_mouse_wheel;
        sdl.on_key_down() = on_key_down;
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

void on_quit()
{
}

void on_idle()
{
    Math::Quaternion Q( Math::Vector( 0, 1.0f, 0 ), Math::to_radians( 1.0f ) );
    object->coordinate_space().rotate( Q );
    scene->render( *camera );

    ++frame_count;
    if (frame_rate_timer.elapsed() > 1000) {
        std::cout << "Frame Rate = " << frame_count << "fps\n";
        frame_rate_timer.restart();
        frame_count = 0;
    }

    // no reason to push the CPU to 100%
    SDL_Delay( 10 );
}

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

void on_mouse_left_down( const Utility::Sdl_manager::Mouse_point& p )
{
    left_button_down = true;
    left_down_point = p;
}

void on_mouse_left_up( const Utility::Sdl_manager::Mouse_point&  )
{
    left_button_down = false;
}

void on_mouse_wheel( int y )
{
    int delta = y*-5;
    if( camera->z_axis_offset() + delta < 5 ) {
        return;
    }
    camera->z_axis_offset() += delta;
}

void on_key_down( SDL_Keycode key, unsigned short mod )
{
    switch( key )
    {
    case SDLK_q:
        sdl.stop();
        break;
    case SDLK_r:
        if (object->renderer() == simple_renderer) {
            object->renderer() = outlined_renderer;
        }
        else {
            object->renderer() = simple_renderer;
        }
        break;
    }
}

