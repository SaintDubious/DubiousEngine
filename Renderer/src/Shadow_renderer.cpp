#include "Shadow_renderer.h"
#include "Open_gl_attributes.h"
#include "Open_gl_primitive.h"
#include "Open_gl_matrix.h"
#include "Open_gl_commands.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

//////////////////////////////////////////////////////////////
namespace {
void draw_shadow_volume( const Math::Local_point& shadow_end, const Math::Local_vector& offset, const Visible_object::Silhouette& sil )
{
    Math::Local_point position = sil.position + offset;
    for (const auto& edge : sil.edges) {
        Open_gl_primitive Prim( Open_gl_primitive::TRIANGLE_STRIP );
        Math::Local_vector p1 = edge.first - Math::Local_point();
        Math::Local_vector p2 = edge.second - Math::Local_point();

        Prim.vertex( position );
        Prim.vertex( position+p2 );
        Prim.vertex( position+p1 );
        Prim.vertex( shadow_end );
    }
    for (const auto& kid : sil.kids) {
        draw_shadow_volume( shadow_end, position - Math::Local_point(), kid );
    }
}
}

//////////////////////////////////////////////////////////////
void Shadow_renderer::render_shadow_volume( const std::shared_ptr<Visible_object>& object, const Math::Local_point& ligh_position )
{
    Visible_object::Silhouette sil;
    object->build_silhouette( ligh_position, sil );
    
    Open_gl_matrix matrix;
//	Open_gl_commands::Translate( pObject->CoordinateSpace().Position() );
    float m[16];
    object->coordinate_space().get_matrix( m );
    Open_gl_commands::mult_matrix( m );

    Math::Local_vector light_vect = Math::Local_unit_vector(ligh_position - Math::Local_point());
    light_vect = light_vect * -100;

    Open_gl_commands::cull_face( GL_FRONT );
    Open_gl_attributes::stencil_op( GL_KEEP, GL_INCR, GL_KEEP );
    draw_shadow_volume( Math::Local_point()+light_vect, Math::Local_vector(), sil );

    Open_gl_commands::cull_face( GL_BACK );
    Open_gl_attributes::stencil_op( GL_KEEP, GL_DECR, GL_KEEP );
    draw_shadow_volume( Math::Local_point()+light_vect, Math::Local_vector(), sil );
}

}
}
