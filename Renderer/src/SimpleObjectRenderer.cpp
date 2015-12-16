#include "SimpleObjectRenderer.h"
#include "VisibleObject.h"
#include "Model.h"
#include "OpenGLContextStore.h"
#include "OpenGLMatrix.h"
#include "OpenGLCommands.h"
#include "OpenGLPrimitive.h"

//////////////////////////////////////////////////////////////
using Dubious::Renderer::SimpleObjectRenderer;
using Dubious::Renderer::LocalPointVector;
using Dubious::Renderer::OpenGLContextStorePtr;
using Dubious::Renderer::VisibleObjectPtr;
using Dubious::Renderer::ModelPtr;
using Dubious::Math::LocalPoint;

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
//	OpenGLCommands::Translate( pObject->CoordinateSpace().Position() );
	float m[16];
	pObject->CoordinateSpace().GetMatrix( m );
	OpenGLCommands::MultMatrix( m );
	OpenGLCommands::CallList( GLHandle );	
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

//////////////////////////////////////////////////////////////
void SimpleObjectRenderer::RenderModel( ModelPtr pModel )
{
	OpenGLMatrix LocalMatrix;
	OpenGLCommands::Translate( pModel->Offset() - LocalPoint() );
	const LocalPointVector& Points( pModel->Points() );
	const Model::SurfaceVector& Surfaces( pModel->Surfaces() );
	OpenGLCommands::Material( GL_AMBIENT, pModel->MaterialColor() );
	OpenGLCommands::Material( GL_DIFFUSE, pModel->MaterialColor() );
	for( size_t i=0; i<Surfaces.size(); ++i ){
		const Math::LocalPoint& A( Points[Surfaces[i].p0] );
		const Math::LocalPoint& B( Points[Surfaces[i].p1] );
		const Math::LocalPoint& C( Points[Surfaces[i].p2] );
		const Math::LocalUnitVector& Normal = Surfaces[i].Normal;

		OpenGLPrimitive Prim( OpenGLPrimitive::TRIANGLES );
		Prim.Normal( Normal );
		Prim.Vertex( A );
		Prim.Vertex( B );
		Prim.Vertex( C );
	}
	const ModelPtrVector& Kids = pModel->Kids();
	for( size_t i=0; i<Kids.size(); ++i )
		RenderModel( Kids[i] );
}

