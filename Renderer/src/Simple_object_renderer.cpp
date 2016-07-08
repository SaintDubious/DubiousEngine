#include "Simple_object_renderer.h"
#include "Visible_object.h"
#include "Visible_model.h"
#include "Open_gl_context_store.h"
#include "Open_gl_matrix.h"
#include "Open_gl_commands.h"
#include "Open_gl_primitive.h"

#include <Vector_math.h>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

//////////////////////////////////////////////////////////////
Simple_object_renderer::Simple_object_renderer( const std::shared_ptr<Open_gl_context_store>& context_store )
    : m_context_store( context_store )
{
}

//////////////////////////////////////////////////////////////
void Simple_object_renderer::render( const std::shared_ptr<Visible_object>& object )
{
    auto iter = m_object_data_map.find( object );
    if (iter == m_object_data_map.end()) {
        m_object_data_map[object] = 0;
        construct_model(object);
        iter = m_object_data_map.find( object );
    }
    if (!m_context_store->call_list_exists( iter->second )) {
        construct_model(object);
    }

    unsigned int gl_handle = m_context_store->get_call_list( iter->second );
    Open_gl_matrix gl_matrix;
    Open_gl_commands::translate( Math::to_vector(object->coordinate_space().position()) );
    float m[16];
    object->coordinate_space().get_matrix( m );
    Open_gl_commands::mult_matrix( m );
    Open_gl_commands::material( GL_AMBIENT, object->base_color() );
    Open_gl_commands::material( GL_DIFFUSE, object->base_color() );
    Open_gl_commands::call_list( gl_handle );	
}

//////////////////////////////////////////////////////////////
namespace {
void render_model( Visible_model& model )
{
    Open_gl_matrix LocalMatrix;
    Open_gl_commands::translate( Math::to_vector(model.offset()) );
    const std::vector<Math::Local_point>& points( model.points() );
    for (const auto& surface : model.surfaces()) {
        const Math::Local_point& a( points[surface.p0] );
        const Math::Local_point& b( points[surface.p1] );
        const Math::Local_point& c( points[surface.p2] );
        const Math::Local_unit_vector& normal = surface.normal;

        Open_gl_primitive prim( Open_gl_primitive::TRIANGLES );
        prim.normal( normal );
        prim.vertex( a );
        prim.vertex( b );
        prim.vertex( c );
    }
    for (const auto& kid : model.kids())
        render_model( *kid );
}
}

//////////////////////////////////////////////////////////////
void Simple_object_renderer::construct_model( const std::shared_ptr<Visible_object>& object )
{
    Call_list_handle handle = m_context_store->create_call_list( 1 );
    unsigned int gl_handle = m_context_store->get_call_list( handle );
    Open_gl_commands::new_list( gl_handle, GL_COMPILE );

    render_model( *object->model() );
    Open_gl_commands::end_list();
    m_object_data_map[object] = handle;
}

}}
