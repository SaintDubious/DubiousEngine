#include "OutlinedObjectRenderer.h"
#include "VisibleObject.h"
#include "OpenGLContextStore.h"
#include "OpenGLMatrix.h"
#include "OpenGLCommands.h"
#include "OpenGLPrimitive.h"
#include "OpenGLAttributes.h"
#include "VisibleModel.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

//////////////////////////////////////////////////////////////
OutlinedObjectRenderer::OutlinedObjectRenderer( OpenGLContextStorePtr pContextStore )
    : m_pContextStore( pContextStore )
{
}

//////////////////////////////////////////////////////////////
void OutlinedObjectRenderer::RenderObject( VisibleObjectPtr pVisibleObject )
{
    // make sure the models have been created and exist in
    // the data maps
    auto iter = m_ObjectDataMap.find( pVisibleObject );
    if (iter == m_ObjectDataMap.end()){
        m_ObjectDataMap[pVisibleObject] = 0;
        m_FlatModelMap[pVisibleObject] = 0;
        ConstructModel(pVisibleObject);
        iter = m_ObjectDataMap.find( pVisibleObject );
    }
    if (!m_pContextStore->CallListExists( iter->second )) {
        ConstructModel(pVisibleObject);
    }

    // retrieve the models from the ContextStore
    unsigned int GLHandle		= m_pContextStore->GetCallList( iter->second );
    unsigned int GLFlatHandle	= m_pContextStore->GetCallList( m_FlatModelMap[pVisibleObject] ); 

    // move the GL Context to the correct position and rotation
    OpenGLMatrix GLMatrix;
    OpenGLCommands::Translate( pVisibleObject->CoordinateSpace().Position() - Math::Point() );
    float m[16];
    pVisibleObject->CoordinateSpace().GetMatrix( m );
    OpenGLCommands::MultMatrix( m );

    // draw the actual object
    OpenGLAttributes Attribs( OpenGLAttributes::ENABLE_BIT | OpenGLAttributes::HINT_BIT | OpenGLAttributes::POLYGON_BIT, false );
    Attribs.Enable( OpenGLAttributes::LINE_SMOOTH );
    Attribs.Enable( OpenGLAttributes::CULL_FACE );
    OpenGLCommands::CullFace (GL_BACK);					
    OpenGLCommands::PolygonMode( GL_BACK, GL_FILL ); 
    Attribs.Enable( OpenGLAttributes::BLEND );
    OpenGLCommands::CallList( GLHandle );	

    // draw the outline
    OpenGLCommands::LineWidth( 4 );
    OpenGLCommands::PolygonMode( GL_BACK, GL_LINE );
    OpenGLCommands::CullFace( GL_FRONT );
    Attribs.DepthFunc (GL_LEQUAL);
    OpenGLCommands::CallList( GLFlatHandle );	

    // reset some of the quirky settings
    // failure to do this results in weirdness
    OpenGLCommands::PolygonMode( GL_BACK, GL_FILL );
}

//////////////////////////////////////////////////////////////
namespace {
void RenderModel( VisibleModelPtr pModel, bool OutlinePass )
{
    OpenGLMatrix LocalMatrix;
    OpenGLCommands::Translate( pModel->Offset() - Math::LocalPoint() );
    const Math::LocalPointVector& Points( pModel->Points() );
    if (!OutlinePass) {
        OpenGLCommands::Material( GL_AMBIENT, pModel->MaterialColor() );
        OpenGLCommands::Material( GL_DIFFUSE, pModel->MaterialColor() );
    }
    for (const VisibleModel::Surface& Surface : pModel->Surfaces()) {
        const Math::LocalPoint& A( Points[Surface.p0] );
        const Math::LocalPoint& B( Points[Surface.p1] );
        const Math::LocalPoint& C( Points[Surface.p2] );
        const Math::LocalUnitVector& NA( Surface.Normal );
        const Math::LocalUnitVector& NB( Surface.Normal );
        const Math::LocalUnitVector& NC( Surface.Normal );

        OpenGLPrimitive Prim( OpenGLPrimitive::TRIANGLES );
        if (!OutlinePass)
            Prim.Normal( NA );
        Prim.Vertex( A );

        if (!OutlinePass)
            Prim.Normal( NB );
        Prim.Vertex( B );

        if (!OutlinePass)
            Prim.Normal( NC );
        Prim.Vertex( C );
    }
    for (const VisibleModelPtr Kid : pModel->Kids())
        RenderModel( Kid, OutlinePass );
}
}

//////////////////////////////////////////////////////////////
void OutlinedObjectRenderer::ConstructModel( VisibleObjectPtr pVisibleObject )
{
    OpenGLAttributes Attribs( OpenGLAttributes::ENABLE_BIT, false );
    {
        CallListHandle Handle = m_pContextStore->CreateCallList( 1 );
        unsigned int GLHandle = m_pContextStore->GetCallList( Handle );
        OpenGLCommands::NewList( GLHandle, GL_COMPILE );
        Attribs.Disable( OpenGLAttributes::TEXTURE_2D );
        RenderModel( pVisibleObject->Model(), false );
        OpenGLCommands::EndList();
        m_ObjectDataMap[pVisibleObject] = Handle;
    }
    {
        CallListHandle Handle = m_pContextStore->CreateCallList( 1 );
        unsigned int GLHandle = m_pContextStore->GetCallList( Handle );
        OpenGLCommands::NewList( GLHandle, GL_COMPILE );
        Attribs.Disable( OpenGLAttributes::TEXTURE_2D );
        OpenGLCommands::Material( GL_AMBIENT, Color::BLACK );
        OpenGLCommands::Material( GL_DIFFUSE, Color::BLACK );
        RenderModel( pVisibleObject->Model(), true );
        OpenGLCommands::EndList();
        m_FlatModelMap[pVisibleObject] = Handle;
    }
}

}
}
