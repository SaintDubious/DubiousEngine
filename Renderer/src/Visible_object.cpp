#include "Visible_object.h"
#include "Visible_model.h"
#include <Vector_math.h>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

//////////////////////////////////////////////////////////////
Visible_object::Visible_object( const std::shared_ptr<Visible_model>& model, const std::shared_ptr<Visible_model>& shadow_model )
    : m_model( model )
    , m_shadow_model( shadow_model )
    , m_base_color( model->color() )
{
}

//////////////////////////////////////////////////////////////
namespace {
void build( const Visible_model& model, const Math::Local_point &light_pos, Visible_object::Silhouette &sil )
{
    sil.position = model.offset();
    const std::vector<Visible_model::Edge>& edges = model.edges();
    if( !edges.empty() ){
        // precompute dot products so as to do less multiplies
        Math::Local_unit_vector light_pos_vector( light_pos.x(), light_pos.y(), light_pos.z() );
        std::vector<float> dot_products;
        for (const auto& surf : model.surfaces()) {
            dot_products.push_back( dot_product( light_pos_vector, surf.normal ) );
        }

        const std::vector<Math::Local_point>& points = model.points();
        for (const auto& edge : edges) {
            float dot1 = dot_products[edge.s0];
            float dot2 = dot_products[edge.s1];
            Visible_object::Silhouette::Edge new_edge;
            if( dot1 <= 0 && dot2 > 0 ) {
                new_edge.first = points[edge.p0];
                new_edge.second = points[edge.p1];
                sil.edges.push_back( new_edge );
            }
            else if( dot1 > 0 && dot2 <= 0 ){
                new_edge.first = points[edge.p1];
                new_edge.second = points[edge.p0];
                sil.edges.push_back( new_edge );
            }
        }
    }
    // have you checked the children?
    for (const auto& kid : model.kids()) {
        sil.kids.push_back( Visible_object::Silhouette() );
        build( *kid, light_pos, sil.kids.back() );
    }
}
}

//////////////////////////////////////////////////////////////
void Visible_object::build_silhouette(const Math::Local_point &light_pos, Silhouette &sil) const
{
    sil.edges.clear();
    sil.kids.clear();
    if (!m_shadow_model)
        return;

    build( *m_shadow_model, light_pos, sil );
}

}}
