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
#include <Open_gl_commands.h>
#include <Open_gl_attributes.h>
#include <Open_gl_primitive.h>
#include <Open_gl_matrix.h>
#include <Vector_math.h>
#include <Timer.h>
#include <Arena.h>
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
const int NUM_OBJECTS = 10;
const int FIRST_OBJECT = 1; // the floor is item 0

//////////////////////////////////////////////////////////////
// Events
void on_quit();
void on_idle();
void on_mouse_motion( const Utility::Sdl_manager::Mouse_point& P );
void on_mouse_right_down( const Utility::Sdl_manager::Mouse_point& P );
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
Utility::Timer                      frame_timer;

Physics::Arena                      arena( 1.0f/60.0f );
std::vector<std::shared_ptr<Renderer::Visible_object>>  visible_objects;
std::vector<std::shared_ptr<Physics::Physics_object>>   physics_objects;

//////////////////////////////////////////////////////////////
int main( int argc, char** argv )
{
    try {
        std::cout << "Starting test\n";

        auto floor_file = Utility::Ac3d_file_reader::test_cube( 10.0f, 0.5f, 10.0f );
        std::unique_ptr<const Utility::Ac3d_file> model_file;
        if (argc == 2) {
            model_file = Utility::Ac3d_file_reader::read_file( Utility::File_path( argv[1] ) );
        }
        else {
            model_file = Utility::Ac3d_file_reader::test_cube( 0.5f, 0.5f, 0.5f );
        }

        sdl.create_root_window( "Physics Viewer", WIDTH, HEIGHT, false );

        std::shared_ptr<Renderer::Open_gl_context_store> context_store = std::make_shared<Renderer::Open_gl_context_store>();
        scene = std::make_unique<Renderer::Scene>( context_store );
        simple_renderer.reset( new Renderer::Simple_object_renderer( context_store ) );

        auto visible_model = std::make_shared<Renderer::Visible_model>( *floor_file, false );
        auto physics_model = std::make_shared<Physics::Physics_model>( *floor_file );
        visible_objects.push_back( std::make_shared<Renderer::Visible_object>( visible_model, visible_model ) );
        visible_objects.back()->coordinate_space().translate( Math::Vector( 0, -0.5f, 0 ) );
        visible_objects.back()->renderer() = simple_renderer;
        scene->add_object( visible_objects.back() );

        physics_objects.push_back( std::make_shared<Physics::Physics_object>( physics_model, Physics::Physics_object::STATIONARY ) );
        physics_objects.back()->coordinate_space().translate( Math::Vector( 0, -0.5f, 0 ) );
        arena.push_back( physics_objects.back() );

        visible_model = std::make_shared<Renderer::Visible_model>( *model_file, false );
        physics_model = std::make_shared<Physics::Physics_model>( *model_file );
        Renderer::Color object_color = Renderer::Color::RED;
        for (int i=0; i<NUM_OBJECTS; ++i) {
            visible_objects.push_back( std::make_shared<Renderer::Visible_object>( visible_model, visible_model ) );
            visible_objects.back()->base_color() = object_color;
            if (object_color == Renderer::Color::RED) {
                object_color = Renderer::Color::BLUE;
            }
            else {
                object_color = Renderer::Color::RED;
            }
            visible_objects.back()->coordinate_space().translate( Math::Vector( 0, i*1.1f+0.55f, 0 ) );
//            visible_objects.back()->coordinate_space().rotate( Math::Quaternion( Math::Unit_vector(0,1,0), Math::to_radians(i*45.0f) ) );
            visible_objects.back()->renderer() = simple_renderer;
            scene->add_object( visible_objects.back() );

            physics_objects.push_back( std::make_shared<Physics::Physics_object>( physics_model, 1.0f ) );
            physics_objects.back()->coordinate_space().translate( Math::Vector( 0, i*1.1f+0.55f, 0 ) );
//            physics_objects.back()->coordinate_space().rotate( Math::Quaternion( Math::Unit_vector(0,1,0), Math::to_radians(i*45.0f) ) );
            arena.push_back( physics_objects.back() );
        }

        scene->scene_light().position   = Math::Point( 20, 20, 0 );
        scene->scene_light().ambient    = Renderer::Color( 0.2f, 0.2f, 0.2f, 1.0f );
        scene->scene_light().diffuse    = Renderer::Color( 0.5f, 0.5f, 0.5f, 1.0f );
        scene->scene_light().specular   = Renderer::Color( 0.1f, 0.1f, 0.1f, 1.0f );

        camera = std::make_unique<Renderer::Camera>( 0, 0, WIDTH,HEIGHT, 80.0f );
        camera->z_axis_offset() = 20;

        sdl.on_quit()                   = on_quit;
        sdl.on_idle()                   = on_idle;
        sdl.on_mouse_motion()           = on_mouse_motion;
        sdl.on_mouse_right_down()       = on_mouse_right_down;
        sdl.on_mouse_left_down()        = on_mouse_left_down;
        sdl.on_mouse_left_up()          = on_mouse_left_up;
        sdl.on_mouse_wheel()            = on_mouse_wheel;
        sdl.on_key_down()               = on_key_down;

        frame_timer.start();

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
    arena.run_physics( frame_timer.restart()/1000.0f );

    for (int i=1; i<NUM_OBJECTS+1; ++i) {
        Math::Point new_position         = physics_objects[i]->coordinate_space().position();
        Math::Quaternion new_orientation = physics_objects[i]->coordinate_space().rotation();
        if (new_position.y() < 0.0f) {
            new_position = Math::Point( new_position.x(), 0.0f, new_position.z() );
            physics_objects[i]->coordinate_space().position() = new_position;
            physics_objects[i]->velocity() = Math::Vector();
        }
        visible_objects[i]->coordinate_space().position() = new_position;
        visible_objects[i]->coordinate_space().rotation() = new_orientation;

        // reset forces to default
        physics_objects[i]->force()  = Math::Vector( 0, -10.0f, 0 );
//        physics_objects[i]->force()  = Math::Vector( 0, 0, 0 );
        physics_objects[i]->torque() = Math::Vector();
    }

    scene->render( *camera );


    // Draw the contact info on top
    Renderer::Open_gl_attributes attribs( Renderer::Open_gl_attributes::ENABLE_BIT | Renderer::Open_gl_attributes::HINT_BIT | Renderer::Open_gl_attributes::POLYGON_BIT, false );
    Renderer::Open_gl_commands::line_width( 2 );
    Renderer::Open_gl_commands::polygon_mode( GL_BACK, GL_LINE );
    Renderer::Open_gl_commands::cull_face( GL_FRONT );
    attribs.depth_func( GL_ALWAYS );

    glColor3f( 1.0f, 0, 0 );
    glPointSize( 5.0f );

    for (const auto& manifold : arena.manifolds()) {
        for (const auto& c : manifold.second.contacts()) {
            {
                Renderer::Open_gl_primitive prim( Renderer::Open_gl_primitive::POINTS );
                prim.vertex( c.contact_point_a );
                prim.vertex( c.contact_point_b );
            }
            Renderer::Open_gl_primitive prim( Renderer::Open_gl_primitive::LINES );
            prim.vertex( c.contact_point_a );
            prim.vertex( c.contact_point_a + Math::Vector(c.normal)*c.penetration_depth );
            /*
            // render tangents
            prim.vertex( c.contact_point_a );
            prim.vertex( c.contact_point_a + Math::Vector(c.tangent1) );
            prim.vertex( c.contact_point_a );
            prim.vertex( c.contact_point_a + Math::Vector(c.tangent2) );
            */
        }

    }

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
void on_mouse_right_down( const Utility::Sdl_manager::Mouse_point& p )
{
    physics_objects[FIRST_OBJECT]->force() = physics_objects[FIRST_OBJECT]->force() + Math::Vector( 0, 500, 0 );
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
    case SDLK_a:
        physics_objects[FIRST_OBJECT]->torque() = Math::Vector( 1, 0, 0 );
        break;
    case SDLK_f:
    physics_objects[FIRST_OBJECT]->force() = physics_objects[FIRST_OBJECT]->force() + Math::Vector( 100, 0, 50 );
        break;
    case SDLK_s:
        physics_objects[FIRST_OBJECT]->torque() = Math::Vector( static_cast<float>(rand())/RAND_MAX, 
                                                     static_cast<float>(rand())/RAND_MAX, 
                                                     static_cast<float>(rand())/RAND_MAX );
        break;
    }
}

