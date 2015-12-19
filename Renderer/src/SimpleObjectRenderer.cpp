#include "SimpleObjectRenderer.h"
#include "VisibleObject.h"
#include "VisibleModel.h"
#include "OpenGLContextStore.h"
#include "OpenGLMatrix.h"
#include "OpenGLCommands.h"
#include "OpenGLPrimitive.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

//////////////////////////////////////////////////////////////
SimpleObjectRenderer::SimpleObjectRenderer( OpenGLContextStorePtr pContextStore )
	: m_pContextStore( pContextStore )
{
}

//////////////////////////////////////////////////////////////
void SimpleObjectRenderer::RenderObject( VisibleObjectPtr pObject )
{
	auto iter = m_ObjectDataMap.find( pObject );
	if( iter == m_ObjectDataMap.end() ){
		m_ObjectDataMap[pObject] = 0;
		ConstructModel(pObject);
		iter = m_ObjectDataMap.find( pObject );
	}
	if( !m_pContextStore->CallListExists( iter->second ) )
		ConstructModel(pObject);

	unsigned int GLHandle = m_pContextStore->GetCallList( iter->second );
	OpenGLMatrix GLMatrix;
	OpenGLCommands::Translate( pObject->CoordinateSpace().Position() - Math::Point() );
	float m[16];
	pObject->CoordinateSpace().GetMatrix( m );
	OpenGLCommands::MultMatrix( m );
	OpenGLCommands::CallList( GLHandle );	
}

//////////////////////////////////////////////////////////////
namespace {
void RenderModel( VisibleModelPtr pModel )
{
    OpenGLMatrix LocalMatrix;
    OpenGLCommands::Translate( pModel->Offset() - Math::LocalPoint() );
    const Math::LocalPointVector& Points( pModel->Points() );
    OpenGLCommands::Material( GL_AMBIENT, pModel->MaterialColor() );
    OpenGLCommands::Material( GL_DIFFUSE, pModel->MaterialColor() );
    for (const VisibleModel::Surface& Surface : pModel->Surfaces()) {
        const Math::LocalPoint& A( Points[Surface.p0] );
        const Math::LocalPoint& B( Points[Surface.p1] );
        const Math::LocalPoint& C( Points[Surface.p2] );
        const Math::LocalUnitVector& Normal = Surface.Normal;

        OpenGLPrimitive Prim( OpenGLPrimitive::TRIANGLES );
        Prim.Normal( Normal );
        Prim.Vertex( A );
        Prim.Vertex( B );
        Prim.Vertex( C );
    }
    for (const VisibleModelPtr Kid : pModel->Kids())
        RenderModel( Kid );
}
}

//////////////////////////////////////////////////////////////
void SimpleObjectRenderer::ConstructModel( VisibleObjectPtr pObject )
{
	CallListHandle Handle = m_pContextStore->CreateCallList( 1 );
	unsigned int GLHandle = m_pContextStore->GetCallList( Handle );
	OpenGLCommands::NewList( GLHandle, GL_COMPILE );

	RenderModel( pObject->Model() );
	OpenGLCommands::EndList();
	m_ObjectDataMap[pObject] = Handle;
}

}}
