#include "Scene.h"
#include "Open_gl_context_store.h"
#include "Open_gl_commands.h"
#include "Open_gl_attributes.h"
#include "Camera.h"
#include "Object_renderer.h"
#include "Shadow_renderer.h"
#include "Visible_object.h"

#include <Vector_math.h>

namespace Dubious {
namespace Renderer {

Scene::Scene( const std::shared_ptr<Open_gl_context_store>& store )
    : m_context_store( store )
{
}

void Scene::render( Camera& camera )
{
    Open_gl_attributes::clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
    camera.ready_for_photo();

    Open_gl_attributes attribs( Open_gl_attributes::ENABLE_BIT | Open_gl_attributes::DEPTH_BUFFER_BIT | Open_gl_attributes::POLYGON_BIT, true );
    setup_light( attribs, true );
    attribs.enable( Open_gl_attributes::DEPTH_TEST );
    Open_gl_commands::cull_face( GL_FRONT );

    // step 1 is draw the scene in shadow
    attribs.depth_func( GL_LEQUAL );
    for (auto& object : m_objects) {
        std::shared_ptr<Object_renderer> renderer = object->renderer();
        if (renderer) {
            renderer->render( object );
        }
    }

    // step 2 is draw the shadow volume
    attribs.enable( Open_gl_attributes::STENCIL_TEST );
    attribs.stencil_func( GL_ALWAYS, 0, 0xffffffff );
    attribs.enable( Open_gl_attributes::DEPTH_TEST );
    attribs.depth_func( GL_LESS );

    attribs.color_mask( false, false, false, false );
    attribs.depth_mask( false );

    attribs.enable( Open_gl_attributes::CULL_FACE );
    for (auto& object : m_objects) {
        std::shared_ptr<Shadow_renderer> renderer = object->shadow_renderer();
        if (renderer) {
            renderer->render_shadow_volume( object, Math::to_point(object->coordinate_space().transform( Math::to_vector(m_scene_light.position) )) );
        }
    }
    attribs.color_mask( true, true, true, true );
    attribs.depth_func( GL_EQUAL );
    attribs.stencil_func( GL_EQUAL, 0, 0xffffffff );
    attribs.stencil_op( GL_KEEP, GL_KEEP, GL_KEEP );

    attribs.enable( Open_gl_attributes::BLEND );
    attribs.blend_func( GL_ONE, GL_ZERO );
    Open_gl_commands::cull_face( GL_BACK );

    // step 3. Draw the scene again in light
    setup_light( attribs, false );
    for (auto& object : m_objects) {
        std::shared_ptr<Object_renderer> renderer = object->renderer();
        if (renderer) {
            renderer->render( object );
        }
    }
}

void Scene::add_object( const std::shared_ptr<Visible_object>& object )
{
    m_objects.push_back( object );
}

void Scene::remove_object( const std::shared_ptr<Visible_object>& object )
{
    m_objects.remove( object );
}

void Scene::remove_all_objects()
{
    m_objects.clear();
}

void Scene::setup_light( Open_gl_attributes& attribs, bool in_shadow ) const
{
    attribs.enable( Open_gl_attributes::LIGHTING );
    attribs.enable( Open_gl_attributes::LIGHT0 );

    attribs.light_position( Open_gl_attributes::LIGHT0, m_scene_light.position );
    if( in_shadow ){
        attribs.light_color( Open_gl_attributes::LIGHT0, GL_AMBIENT, m_scene_light.ambient );
        attribs.light_color( Open_gl_attributes::LIGHT0, GL_DIFFUSE, Color::BLACK );
        attribs.light_color( Open_gl_attributes::LIGHT0, GL_SPECULAR, Color::BLACK );
    } else {
        attribs.light_color( Open_gl_attributes::LIGHT0, GL_AMBIENT, m_scene_light.ambient );
        attribs.light_color( Open_gl_attributes::LIGHT0, GL_DIFFUSE, m_scene_light.diffuse );
        attribs.light_color( Open_gl_attributes::LIGHT0, GL_SPECULAR, m_scene_light.specular );
    }
    /*
    float LightParams[4];
    LightParams[0] = 0;
    LightParams[1] = 0;
    LightParams[2] = 0;
    LightParams[3] = 1.0;
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, LightParams );
    */
}

}}
