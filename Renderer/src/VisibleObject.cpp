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
    if( !m_pShadowModel )
        return;

    BuildSilhouette( m_pShadowModel, LightPos, Sil );
}

//////////////////////////////////////////////////////////////
void VisibleObject::BuildSilhouette( VisibleModelPtr pModel, const Math::LocalPoint &LightPos, Silhouette &Sil) const
{
    Sil.Position = pModel->Offset();
    const VisibleModel::EdgeVector& Edges = pModel->Edges();
    if( !Edges.empty() ){
        const Math::LocalPointVector& Points = pModel->Points();
        const VisibleModel::SurfaceVector Surfaces = pModel->Surfaces();
        const Math::LocalPoint* pVertices = &(Points[0]);
        const VisibleModel::Surface* pSurfaces = &(Surfaces[0]);

        // precompute dot products so as to do less multiplies
        Math::LocalUnitVector LightPosVector( LightPos.X(), LightPos.Y(), LightPos.Z() );
        std::vector<float> DotProducts( Surfaces.size() );
        for( size_t i=0; i<Surfaces.size(); ++i )
            DotProducts[i] = DotProduct( LightPosVector, pSurfaces[i].Normal );

        Sil.Edges.reserve( 20 );
        // This is sort of a hack, but the STL is a bit too slow to pull this off
        // in debug mode, so dropping down to raw arrays.
        const float* pDots = &(DotProducts[0]);
        const VisibleModel::Edge* pEdges = &(Edges[0]);
        for( size_t i=0; i<Edges.size(); ++i ){
            float Dot1 = pDots[pEdges[i].s0];
            float Dot2 = pDots[pEdges[i].s1];
            Silhouette::Edge NewEdge;
            if( Dot1 <= 0 && Dot2 > 0 ) {
                NewEdge.first = pVertices[pEdges[i].p0];
                NewEdge.second = pVertices[pEdges[i].p1];
                Sil.Edges.push_back( NewEdge );
            }
            else if( Dot1 > 0 && Dot2 <= 0 ){
                NewEdge.first = pVertices[pEdges[i].p1];
                NewEdge.second = pVertices[pEdges[i].p0];
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
