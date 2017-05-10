#include "Outlined_object_renderer.h"
#include "Visible_object.h"
#include "Open_gl_context_store.h"
#include "Open_gl_matrix.h"
#include "Open_gl_commands.h"
#include "Open_gl_primitive.h"
#include "Open_gl_attributes.h"
#include "Visible_model.h"

#include <Vector_math.h>

namespace Dubious {
namespace Renderer {

Outlined_object_renderer::Outlined_object_renderer( const std::shared_ptr<Open_gl_context_store>& context_store )
    : m_context_store( context_store )
{
}

void Outlined_object_renderer::render( const std::shared_ptr<Visible_object>& object )
{
    // make sure the models have been created and exist in
    // the data maps
    auto iter = m_object_data_map.find( object );
    if (iter == m_object_data_map.end()){
        m_object_data_map[object] = 0;
        m_flat_model_map[object] = 0;
        construct_model(object);
        iter = m_object_data_map.find( object );
    }
    if (!m_context_store->call_list_exists( iter->second )) {
        construct_model(object);
    }

    // retrieve the models from the ContextStore
    unsigned int gl_handle		= m_context_store->get_call_list( iter->second );
    unsigned int gl_flat_handle	= m_context_store->get_call_list( m_flat_model_map[object] ); 

    // move the GL Context to the correct position and rotation
    Open_gl_matrix gl_matrix;
    Open_gl_commands::translate( Math::to_vector(object->coordinate_space().position()) );
    float m[16];
    object->coordinate_space().get_matrix( m );
    Open_gl_commands::mult_matrix( m );

    // draw the actual object
    Open_gl_attributes attribs( Open_gl_attributes::ENABLE_BIT | Open_gl_attributes::HINT_BIT | Open_gl_attributes::POLYGON_BIT, false );
    attribs.enable( Open_gl_attributes::LINE_SMOOTH );
    attribs.enable( Open_gl_attributes::CULL_FACE );
    Open_gl_commands::cull_face( GL_BACK );		
    Open_gl_commands::polygon_mode( GL_BACK, GL_FILL ); 
    attribs.enable( Open_gl_attributes::BLEND );
    Open_gl_commands::call_list( gl_handle );	

    // draw the outline
    Open_gl_commands::line_width( 4 );
    Open_gl_commands::polygon_mode( GL_BACK, GL_LINE );
    Open_gl_commands::cull_face( GL_FRONT );
    attribs.depth_func( GL_LEQUAL );
    Open_gl_commands::call_list( gl_flat_handle );	

    // reset some of the quirky settings
    // failure to do this results in weirdness
    Open_gl_commands::polygon_mode( GL_BACK, GL_FILL );
}

namespace {
void render_model( Visible_model& model, bool outline_pass )
{
    Open_gl_matrix local_matrix;
    Open_gl_commands::translate( Math::to_vector(model.offset()) );
    const std::vector<Math::Local_point>& points( model.points() );
    if (!outline_pass) {
        Open_gl_commands::material( GL_AMBIENT, model.color() );
        Open_gl_commands::material( GL_DIFFUSE, model.color() );
    }
    for (const auto& surface : model.surfaces()) {
        const Math::Local_point& a( points[surface.p0] );
        const Math::Local_point& b( points[surface.p1] );
        const Math::Local_point& c( points[surface.p2] );
        const Math::Local_unit_vector& na( surface.normal );
        const Math::Local_unit_vector& nb( surface.normal );
        const Math::Local_unit_vector& nc( surface.normal );

        Open_gl_primitive prim( Open_gl_primitive::TRIANGLES );
        if (!outline_pass)
            prim.normal( na );
        prim.vertex( a );

        if (!outline_pass)
            prim.normal( nb );
        prim.vertex( b );

        if (!outline_pass)
            prim.normal( nc );
        prim.vertex( c );
    }
    for (const auto& kid : model.kids())
        render_model( *kid, outline_pass );
}
}

void Outlined_object_renderer::construct_model( const std::shared_ptr<Visible_object>& object )
{
    Open_gl_attributes attribs( Open_gl_attributes::ENABLE_BIT, false );
    {
        Call_list_handle handle = m_context_store->create_call_list( 1 );
        unsigned int gl_handle = m_context_store->get_call_list( handle );
        Open_gl_commands::new_list( gl_handle, GL_COMPILE );
        attribs.disable( Open_gl_attributes::TEXTURE_2D );
        render_model( *object->model(), false );
        Open_gl_commands::end_list();
        m_object_data_map[object] = handle;
    }
    {
        Call_list_handle handle = m_context_store->create_call_list( 1 );
        unsigned int gl_handle = m_context_store->get_call_list( handle );
        Open_gl_commands::new_list( gl_handle, GL_COMPILE );
        attribs.disable( Open_gl_attributes::TEXTURE_2D );
        Open_gl_commands::material( GL_AMBIENT, Color::BLACK );
        Open_gl_commands::material( GL_DIFFUSE, Color::BLACK );
        render_model( *object->model(), true );
        Open_gl_commands::end_list();
        m_flat_model_map[object] = handle;
    }
}

}
}
