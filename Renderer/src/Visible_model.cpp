#include "Visible_model.h"
#include <Ac3d_file_reader.h>
#include <Vector_math.h>

namespace Dubious {
namespace Renderer {

Visible_model::Visible_model()
    : m_color( Color::WHITE )
{
}

Visible_model::Visible_model( const Utility::Ac3d_file& file, bool include_edges )
    : m_color( Color::WHITE )
{
    construct( *file.model(), file.materials(), include_edges );
}

namespace {
Visible_model::surface build_surface( const std::vector<Math::Local_point>& points, int p0, int p1, int p2 )
{
    const Math::Local_point& A = points[p0];
    const Math::Local_point& B = points[p1];
    const Math::Local_point& C = points[p2];
    return Visible_model::surface( p0, p1, p2, Math::cross_product( (B-A), (C-A) ) );
}

}

void Visible_model::construct( const Utility::Ac3d_model& ac3d_model, const std::vector<Utility::Ac3d_material>& materials, bool include_edges )
{
    m_offset = ac3d_model.offset();
    m_points = ac3d_model.points();
    for (const auto& s : ac3d_model.surfaces()) {
        m_surfaces.push_back( build_surface( m_points, s.p0, s.p1, s.p2 ) );
    }
    for (const auto& p : ac3d_model.kids()) {
        m_kids.push_back( std::unique_ptr<Visible_model>( new Visible_model() ) );
        m_kids.back()->construct( *p, materials, include_edges );
    }
    if (!ac3d_model.surfaces().empty()) {
        const auto& material = materials[ac3d_model.surfaces()[0].material_index];
        m_color = Color( material.color().red, material.color().green, material.color().blue, 1.0f );
    }
//	compute_point_normals();
    if (include_edges) {
        build_edges();
    }
}

void Visible_model::compute_point_normals()
{
    for( size_t i=0; i<m_points.size(); ++i ){
        Math::Local_vector avg_normal;
        for (const auto& s : m_surfaces) {
            if ((s.p0 == i) || (s.p1 == i) || (s.p2 == i) ){
                // this surface includes the vertex in question.
                // find its normal
                avg_normal = avg_normal + Math::Local_vector( s.normal );
            }
        }
        m_point_normals.push_back( Math::Local_unit_vector( avg_normal ) );
    }
}

void Visible_model::build_edges()
{
    size_t j = 0;
    for( size_t i=0; i<m_surfaces.size(); ++i ){
        unsigned short i0 = m_surfaces[i].p0;
        unsigned short i1 = m_surfaces[i].p1;
        for( j=i+1; j<m_surfaces.size(); ++j ){
            if( ((m_surfaces[j].p0==i0) && ((m_surfaces[j].p1==i1) || (m_surfaces[j].p2==i1))) ||
                ((m_surfaces[j].p1==i0) && ((m_surfaces[j].p0==i1) || (m_surfaces[j].p2==i1))) ||
                ((m_surfaces[j].p2==i0) && ((m_surfaces[j].p0==i1) || (m_surfaces[j].p1==i1))) ){
                add_edge( i0, i1, static_cast<unsigned short>(i), static_cast<unsigned short>(j) );
                break;
            }
        }
        i0 = m_surfaces[i].p1;
        i1 = m_surfaces[i].p2;
        for( j=i+1; j<m_surfaces.size(); ++j ){
            if( ((m_surfaces[j].p0==i0) && ((m_surfaces[j].p1==i1) || (m_surfaces[j].p2==i1))) ||
                ((m_surfaces[j].p1==i0) && ((m_surfaces[j].p0==i1) || (m_surfaces[j].p2==i1))) ||
                ((m_surfaces[j].p2==i0) && ((m_surfaces[j].p0==i1) || (m_surfaces[j].p1==i1))) ){
                add_edge( i0, i1, static_cast<unsigned short>(i), static_cast<unsigned short>(j) );
                break;
            }
        }
        i0 = m_surfaces[i].p2;
        i1 = m_surfaces[i].p0;
        for( j=i+1; j<m_surfaces.size(); ++j ){
            if( ((m_surfaces[j].p0==i0) && ((m_surfaces[j].p1==i1) || (m_surfaces[j].p2==i1))) ||
                ((m_surfaces[j].p1==i0) && ((m_surfaces[j].p0==i1) || (m_surfaces[j].p2==i1))) ||
                ((m_surfaces[j].p2==i0) && ((m_surfaces[j].p0==i1) || (m_surfaces[j].p1==i1))) ){
                add_edge( i0, i1, static_cast<unsigned short>(i), static_cast<unsigned short>(j) );
                break;
            }
        }
    }
}

void Visible_model::add_edge( unsigned short p0, unsigned short p1, unsigned short s0, unsigned short s1 )
{
    Edge	new_edge;
    new_edge.p0 = p0;
    new_edge.p1 = p1;
    new_edge.s0 = s0;
    new_edge.s1 = s1;
    m_edges.push_back( new_edge );
}

}}