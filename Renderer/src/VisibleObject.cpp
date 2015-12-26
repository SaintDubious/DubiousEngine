#include "VisibleObject.h"
#include "VisibleModel.h"
#include <VectorMath.h>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

//////////////////////////////////////////////////////////////
VisibleObject::VisibleObject( VisibleModelPtr pModel, VisibleModelPtr pShadowModel )
    : m_pModel( pModel )
    , m_pShadowModel( pShadowModel )
    , m_BaseColor( pModel->MaterialColor() )
{
}

//////////////////////////////////////////////////////////////
void VisibleObject::BuildSilhouette(const Math::LocalPoint &LightPos, Silhouette &Sil) const
{
    Sil.Edges.clear();
    Sil.Kids.clear();
    if (!m_pShadowModel)
        return;

    BuildSilhouette( m_pShadowModel, LightPos, Sil );
}

//////////////////////////////////////////////////////////////
void VisibleObject::BuildSilhouette( VisibleModelPtr pModel, const Math::LocalPoint &LightPos, Silhouette &Sil) const
{
    Sil.Position = pModel->Offset();
    const VisibleModel::EdgeVector& Edges = pModel->Edges();
    if( !Edges.empty() ){
        // precompute dot products so as to do less multiplies
        Math::LocalUnitVector LightPosVector( LightPos.X(), LightPos.Y(), LightPos.Z() );
        std::vector<float> DotProducts;
        for (const VisibleModel::Surface& Surf : pModel->Surfaces()) {
            DotProducts.push_back( DotProduct( LightPosVector, Surf.Normal ) );
        }

        const Math::LocalPointVector& Points = pModel->Points();
        for (const VisibleModel::Edge& Edge : Edges) {
            float Dot1 = DotProducts[Edge.s0];
            float Dot2 = DotProducts[Edge.s1];
            Silhouette::Edge NewEdge;
            if( Dot1 <= 0 && Dot2 > 0 ) {
                NewEdge.first = Points[Edge.p0];
                NewEdge.second = Points[Edge.p1];
                Sil.Edges.push_back( NewEdge );
            }
            else if( Dot1 > 0 && Dot2 <= 0 ){
                NewEdge.first = Points[Edge.p1];
                NewEdge.second = Points[Edge.p0];
                Sil.Edges.push_back( NewEdge );
            }
        }
    }
    // have you checked the children?
    Sil.Kids.reserve( pModel->Kids().size() );
    for (const VisibleModelPtr Kid : pModel->Kids()) {
        Sil.Kids.push_back( Silhouette() );
        BuildSilhouette( Kid, LightPos, Sil.Kids[Sil.Kids.size()-1] );
    }
}

}}
