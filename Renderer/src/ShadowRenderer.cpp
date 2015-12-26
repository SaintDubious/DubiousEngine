#include "ShadowRenderer.h"
#include "OpenGLAttributes.h"
#include "OpenGLPrimitive.h"
#include "OpenGLMatrix.h"
#include "OpenGLCommands.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

//////////////////////////////////////////////////////////////
namespace {
void DrawShadowVolume( const Math::LocalPoint& ShadowEnd, const Math::LocalVector& Offset, const VisibleObject::Silhouette& Sil )
{
    Math::LocalPoint Position = Sil.Position + Offset;
    for (const VisibleObject::Silhouette::Edge& Edge : Sil.Edges) {
        OpenGLPrimitive Prim( OpenGLPrimitive::TRIANGLE_STRIP );
        Math::LocalVector p1 = Edge.first - Math::LocalPoint();
        Math::LocalVector p2 = Edge.second - Math::LocalPoint();

        Prim.Vertex( Position );
        Prim.Vertex( Position+p2 );
        Prim.Vertex( Position+p1 );
        Prim.Vertex( ShadowEnd );
    }
    for (const VisibleObject::Silhouette& Kid : Sil.Kids) {
        DrawShadowVolume( ShadowEnd, Position - Math::LocalPoint(), Kid );
    }
}
}

//////////////////////////////////////////////////////////////
void ShadowRenderer::RenderShadowVolume( VisibleObjectPtr pObject, const Math::LocalPoint& LightPosition )
{
    VisibleObject::Silhouette Sil;
    pObject->BuildSilhouette( LightPosition, Sil );
    
    OpenGLMatrix Matrix;
//	OpenGLCommands::Translate( pObject->CoordinateSpace().Position() );
    float m[16];
    pObject->CoordinateSpace().GetMatrix( m );
    OpenGLCommands::MultMatrix( m );

    Math::LocalVector LightVect = Math::LocalUnitVector(LightPosition - Math::LocalPoint());
    LightVect = LightVect * -100;

    OpenGLCommands::CullFace( GL_FRONT );
    OpenGLAttributes::StencilOp( GL_KEEP, GL_INCR, GL_KEEP );
    DrawShadowVolume( Math::LocalPoint()+LightVect, Math::LocalVector(), Sil );

    OpenGLCommands::CullFace( GL_BACK );
    OpenGLAttributes::StencilOp( GL_KEEP, GL_DECR, GL_KEEP );
    DrawShadowVolume( Math::LocalPoint()+LightVect, Math::LocalVector(), Sil );
}

}
}
