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
#include <Unit_quaternion.h>

#include <memory>
#include <iostream>

using namespace Dubious;

const float LIGHT_HEIGHT      = 50.0f;
const float PI                = 3.1415926535f;
const int   WIDTH             = 800;
const int   HEIGHT            = 600;
const int   NUM_LAYERS        = 1;
const int   OBJECTS_PER_LAYER = 1;
const float OBJECT_WIDTH      = 0.008f;
const float OBJECT_HEIGHT     = 0.024f;
const float OBJECT_DEPTH      = 0.12f;
const float OBJECT_MASS       = 0.1f;
const int   FIRST_OBJECT      = 1;  // the floor is item 0

// Events
void on_quit();
void on_idle();
void on_mouse_motion(const Utility::Sdl_manager::Mouse_point& P);
void on_mouse_right_down(const Utility::Sdl_manager::Mouse_point& P);
void on_mouse_left_down(const Utility::Sdl_manager::Mouse_point& P);
void on_mouse_left_up(const Utility::Sdl_manager::Mouse_point& P);
void on_mouse_wheel(int Y);
void on_key_down(SDL_Keycode Key, unsigned short Mod);
void build_layer(float y, float angle);

// Globals
Utility::Sdl_manager                              sdl;
std::unique_ptr<Renderer::Scene>                  scene;
std::unique_ptr<Renderer::Camera>                 camera;
Utility::Sdl_manager::Mouse_point                 left_down_point;
bool                                              left_button_down;
std::shared_ptr<Renderer::Simple_object_renderer> simple_renderer;
Utility::Timer                                    frame_timer;
float                                             elapsed;
int                                               frame_count;
bool                                              paused;
bool                                              single_step;

std::unique_ptr<Physics::Arena>                        arena;
std::shared_ptr<Renderer::Visible_model>               visible_model;
std::shared_ptr<Physics::Physics_model>                physics_model;
std::vector<std::shared_ptr<Renderer::Visible_object>> visible_objects;
std::vector<std::shared_ptr<Physics::Physics_object>>  physics_objects;
Renderer::Color                                        object_color;

int
main(int argc, char** argv)
{
    try {
        std::cout << "Starting test\n";

        Physics::Arena::Collision_solver_settings collision_solver_settings;
        collision_solver_settings.strategy =
            Physics::Arena::Collision_solver_settings::Strategy::SINGLE_THREADED;
        collision_solver_settings.manifold_persistent_threshold = 0.00004f;
        collision_solver_settings.greedy_manifold               = true;
        collision_solver_settings.mt_collisions_work_group_size = 500;

        Physics::Arena::Constraint_solver_settings constraint_solver_settings;
        constraint_solver_settings.coefficient_of_restitution = 0.0025f;
        constraint_solver_settings.beta                       = 0.05f;
        constraint_solver_settings.slop                       = 0.0005f;
        constraint_solver_settings.warm_start_scale           = 0.5f;

        arena = std::make_unique<Physics::Arena>(
            Physics::Arena::Settings(collision_solver_settings, constraint_solver_settings));

        paused          = true;
        single_step     = false;
        elapsed         = 0;
        frame_count     = 0;
        auto floor_file = Utility::Ac3d_file_reader::test_cube(3.0f, 0.5f, 3.0f);
        std::unique_ptr<const Utility::Ac3d_file> model_file;
        model_file = Utility::Ac3d_file_reader::test_cube(OBJECT_WIDTH / 2.0f, OBJECT_HEIGHT / 2.0f,
                                                          OBJECT_DEPTH / 2.0f);

        sdl.create_root_window("Physics Viewer", WIDTH, HEIGHT, false);

        std::shared_ptr<Renderer::Open_gl_context_store> context_store =
            std::make_shared<Renderer::Open_gl_context_store>();
        scene = std::make_unique<Renderer::Scene>(context_store);
        simple_renderer.reset(new Renderer::Simple_object_renderer(context_store));

        visible_model = std::make_shared<Renderer::Visible_model>(*floor_file, false);
        physics_model = std::make_shared<Physics::Physics_model>(*floor_file);
        visible_objects.push_back(
            std::make_shared<Renderer::Visible_object>(visible_model, visible_model));
        visible_objects.back()->coordinate_space().translate(Math::Vector(0, -0.5f, 0));
        visible_objects.back()->renderer() = simple_renderer;
        scene->add_object(visible_objects.back());

        physics_objects.push_back(std::make_shared<Physics::Physics_object>(
            physics_model, Physics::Physics_object::STATIONARY));
        physics_objects.back()->coordinate_space().translate(Math::Vector(0, -0.5f, 0));
        arena->push_back(physics_objects.back());

        visible_model = std::make_shared<Renderer::Visible_model>(*model_file, false);
        physics_model = std::make_shared<Physics::Physics_model>(*model_file);
        float layer   = OBJECT_HEIGHT / 2.0f;
        float angle   = 0.0f;
        for (int i = 0; i < NUM_LAYERS; ++i) {
            build_layer(layer, angle);
            layer += OBJECT_HEIGHT;
            if (angle < 0.001f) {
                angle = PI / 4.0f;
            }
            else {
                angle = 0.0f;
            }
        }

        scene->scene_light().position = Math::Point(20, 20, 0);
        scene->scene_light().ambient  = Renderer::Color(0.2f, 0.2f, 0.2f, 1.0f);
        scene->scene_light().diffuse  = Renderer::Color(0.5f, 0.5f, 0.5f, 1.0f);
        scene->scene_light().specular = Renderer::Color(0.1f, 0.1f, 0.1f, 1.0f);

        camera                  = std::make_unique<Renderer::Camera>(0, 0, WIDTH, HEIGHT, 80.0f);
        camera->z_axis_offset() = 2;
        camera->coordinate_space().position() = Math::Point(0, 0.0f, 0);

        sdl.on_quit()             = on_quit;
        sdl.on_idle()             = on_idle;
        sdl.on_mouse_motion()     = on_mouse_motion;
        sdl.on_mouse_right_down() = on_mouse_right_down;
        sdl.on_mouse_left_down()  = on_mouse_left_down;
        sdl.on_mouse_left_up()    = on_mouse_left_up;
        sdl.on_mouse_wheel()      = on_mouse_wheel;
        sdl.on_key_down()         = on_key_down;

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

void
build_layer(float y, float angle)
{
    if (object_color == Renderer::Color::RED) {
        object_color = Renderer::Color::BLUE;
    }
    else {
        object_color = Renderer::Color::RED;
    }
    const Math::Local_vector offset(0.1f, y, 0);
    for (int i = 0; i < OBJECTS_PER_LAYER; ++i) {
        float radians = angle + static_cast<float>(i) * PI / 2.0f;
        // float radians = angle + 2.0f * PI / 2.0f;
        visible_objects.push_back(
            std::make_shared<Renderer::Visible_object>(visible_model, visible_model));
        visible_objects.back()->base_color() = object_color;
        visible_objects.back()->coordinate_space().rotate(
            Math::Unit_quaternion(Math::Unit_vector(0, 1, 0), radians));
        visible_objects.back()->coordinate_space().translate(offset);
        visible_objects.back()->renderer() = simple_renderer;
        scene->add_object(visible_objects.back());
        physics_objects.push_back(
            std::make_shared<Physics::Physics_object>(physics_model, OBJECT_MASS));
        physics_objects.back()->coordinate_space().rotate(
            Math::Unit_quaternion(Math::Unit_vector(0, 1, 0), radians));
        physics_objects.back()->coordinate_space().translate(offset);
        arena->push_back(physics_objects.back());
    }
}

void
on_quit()
{
}

void
on_idle()
{
    elapsed += frame_timer.elapsed();
    ++frame_count;
    if (elapsed > 1000.0f) {
        //        std::cout << frame_count << " fps\n";
        frame_count = 0;
        elapsed     = 0;
    }
    int64_t frame_time = frame_timer.restart();
    if (!paused || single_step) {
        if (single_step) {
            frame_time = 17;
        }
        //       frame_time = 17;
        arena->run_physics(frame_time / 1000.0f);
        single_step = false;
    }
    for (int i = 1; i < NUM_LAYERS * OBJECTS_PER_LAYER + 1; ++i) {
        Math::Point           new_position    = physics_objects[i]->coordinate_space().position();
        Math::Unit_quaternion new_orientation = physics_objects[i]->coordinate_space().rotation();
        //      if (new_position.y() < 0.0f) {
        //          new_position = Math::Point( new_position.x(), 0.0f, new_position.z() );
        //            physics_objects[i]->coordinate_space().position() = new_position;
        //            physics_objects[i]->velocity() = Math::Vector();
        //      }
        visible_objects[i]->coordinate_space().position() = new_position;
        visible_objects[i]->coordinate_space().rotation() = new_orientation;

        // reset forces to default
        physics_objects[i]->force() = Math::Vector(0, -9.8f * OBJECT_MASS, 0);
        //        physics_objects[i]->force()  = Math::Vector( 0, 0, 0 );
        physics_objects[i]->torque() = Math::Vector();
    }

    scene->render(*camera);

    // Draw the contact info on top
    Renderer::Open_gl_attributes attribs(Renderer::Open_gl_attributes::ENABLE_BIT |
                                             Renderer::Open_gl_attributes::HINT_BIT |
                                             Renderer::Open_gl_attributes::POLYGON_BIT,
                                         false);
    Renderer::Open_gl_commands::line_width(2);
    Renderer::Open_gl_commands::polygon_mode(GL_BACK, GL_LINE);
    Renderer::Open_gl_commands::cull_face(GL_FRONT);
    attribs.depth_func(GL_ALWAYS);

    glColor3f(0, 1.0f, 0);
    glPointSize(3.0f);

    for (const auto& manifold : arena->manifolds()) {
        for (const auto& c : manifold.second.contacts()) {
            {
                Renderer::Open_gl_primitive prim(Renderer::Open_gl_primitive::POINTS);
                prim.vertex(c.contact_point_a);
                prim.vertex(c.contact_point_b);
            }
            //            Renderer::Open_gl_primitive prim(Renderer::Open_gl_primitive::LINES);
            //          prim.vertex(c.contact_point_a);
            //        prim.vertex(c.contact_point_a + Math::Vector(c.normal) * c.penetration_depth);
            // render tangents
            //            prim.vertex( c.contact_point_a );
            //            prim.vertex( c.contact_point_a + Math::Vector(c.tangent1) );
            //            prim.vertex( c.contact_point_a );
            //            prim.vertex( c.contact_point_a + Math::Vector(c.tangent2) );
        }
    }
    SDL_Delay(10);
}

void
on_mouse_motion(const Utility::Sdl_manager::Mouse_point& p)
{
    if (left_button_down) {
        Utility::Sdl_manager::Mouse_point offset =
            std::make_pair(left_down_point.first - p.first, left_down_point.second - p.second);
        Math::Unit_quaternion y_rotation(Math::Vector(0, 1.0f, 0),
                                         Math::to_radians(static_cast<float>(offset.first)));
        camera->coordinate_space().rotate(y_rotation);

        // Rotation around the local XAxis is limited to about 80 degrees in
        // either direction.
        Math::Vector      max_y(0, camera->z_axis_offset(), 0);
        Math::Unit_vector z;
        std::tie(std::ignore, std::ignore, z) = camera->coordinate_space().get_axes();
        Math::Vector vz(z);
        vz = vz * camera->z_axis_offset();
        if (offset.second > 0) {
            if (Math::dot_product(max_y, vz) >
                -(camera->z_axis_offset() * camera->z_axis_offset()) * 0.9f) {
                Math::Local_unit_quaternion x_rotation(
                    Math::Local_vector(1.0f, 0, 0),
                    Math::to_radians(static_cast<float>(offset.second)));
                camera->coordinate_space().rotate(x_rotation);
            }
        }
        else if (offset.second < 0) {
            if (Math::dot_product(max_y, vz) <
                (camera->z_axis_offset() * camera->z_axis_offset()) * 0.9f) {
                Math::Local_unit_quaternion x_rotation(
                    Math::Local_vector(1.0f, 0, 0),
                    Math::to_radians(static_cast<float>(offset.second)));
                camera->coordinate_space().rotate(x_rotation);
            }
        }

        left_down_point = p;
    }
}

void
on_mouse_right_down(const Utility::Sdl_manager::Mouse_point& p)
{
}

void
on_mouse_left_down(const Utility::Sdl_manager::Mouse_point& p)
{
    left_button_down = true;
    left_down_point  = p;
}

void
on_mouse_left_up(const Utility::Sdl_manager::Mouse_point&)
{
    left_button_down = false;
}

void
on_mouse_wheel(int y)
{
    float delta = y * -0.2f;
    if (camera->z_axis_offset() + delta < 0.2) {
        return;
    }
    camera->z_axis_offset() += delta;
}

void
on_key_down(SDL_Keycode key, unsigned short mod)
{
    switch (key) {
    case SDLK_q:
        sdl.stop();
        break;
    case SDLK_p:
        paused = !paused;
        break;
    case SDLK_s:
        single_step = true;
        break;
    }
}
