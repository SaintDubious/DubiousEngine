#include "Scene.h"
#include "OpenGLContextStore.h"
#include "OpenGLCommands.h"
#include "OpenGLAttributes.h"
#include "Camera.h"
#include "ObjectRenderer.h"
//#include "ShadowRenderer.h"
#include "VisibleObject.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

typedef std::shared_ptr<ObjectRenderer> ObjectRendererPtr;

//////////////////////////////////////////////////////////////
Scene::Scene( OpenGLContextStorePtr pStore )
	: m_pContextStore( pStore )
{
}

//////////////////////////////////////////////////////////////
void Scene::Render( CameraPtr pCamera )
{
	OpenGLAttributes::Clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	pCamera->ReadyForPhoto();

	OpenGLAttributes Attribs( OpenGLAttributes::ENABLE_BIT | OpenGLAttributes::DEPTH_BUFFER_BIT | OpenGLAttributes::POLYGON_BIT, true );
	SetupLight( Attribs, true );
	Attribs.Enable( OpenGLAttributes::DEPTH_TEST );

	auto iter = m_Objects.cbegin();
	auto end = m_Objects.cend();
	ObjectRendererPtr pRenderer;
	OpenGLCommands::CullFace( GL_FRONT );
	// step 1 is draw the scene in shadow
	Attribs.DepthFunc( GL_LEQUAL );

	for( ; iter!=end; ++iter ){
		pRenderer = (*iter)->ObjectRenderer();
		if( pRenderer )
			pRenderer->RenderObject( *iter );
	}
	// step 2 is draw the shadow volume
	Attribs.Enable( OpenGLAttributes::STENCIL_TEST );
	Attribs.StencilFunc( GL_ALWAYS, 0, 0xffffffff );
	Attribs.Enable( OpenGLAttributes::DEPTH_TEST );
	Attribs.DepthFunc( GL_LESS );

	Attribs.ColorMask( false, false, false, false );
	Attribs.DepthMask( false );

	Attribs.Enable( OpenGLAttributes::CULL_FACE );
	iter = m_Objects.begin();
	for( ; iter!=end; ++iter ){
		ShadowRendererPtr pShadowRenderer = (*iter)->ShadowRenderer();
//		if (pShadowRenderer)
//			pShadowRenderer->RenderShadowVolume( *iter, (*iter)->CoordinateSpace().Transform( m_SceneLight.Position ) );
	}
	Attribs.ColorMask( true, true, true, true );
	Attribs.DepthFunc( GL_EQUAL );
	Attribs.StencilFunc( GL_EQUAL, 0, 0xffffffff );
	Attribs.StencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

	Attribs.Enable( OpenGLAttributes::BLEND );
	Attribs.BlendFunc( GL_ONE, GL_ZERO );
	OpenGLCommands::CullFace( GL_BACK );

	// step 3. Draw the scene again in light
	SetupLight( Attribs, false );
	iter = m_Objects.begin();
	for( ; iter!=end; ++iter ){
		pRenderer = (*iter)->ObjectRenderer();
		if( pRenderer )
			pRenderer->RenderObject( *iter );
	}
}

//////////////////////////////////////////////////////////////
void Scene::AddObject( VisibleObjectPtr pObject )
{
	m_Objects.push_back( pObject );
}

//////////////////////////////////////////////////////////////
void Scene::RemoveObject( VisibleObjectPtr pObject )
{
	m_Objects.remove( pObject );
}

//////////////////////////////////////////////////////////////
void Scene::RemoveAllObjects()
{
	m_Objects.clear();
}

//////////////////////////////////////////////////////////////
void Scene::SetupLight( OpenGLAttributes& Attribs, bool InShadow ) const
{
	Attribs.Enable( OpenGLAttributes::LIGHTING );
	Attribs.Enable( OpenGLAttributes::LIGHT0 );

	Attribs.LightPosition( OpenGLAttributes::LIGHT0, m_SceneLight.Position );
	if( InShadow ){
		Attribs.LightColor( OpenGLAttributes::LIGHT0, GL_AMBIENT, m_SceneLight.Ambient );
		Attribs.LightColor( OpenGLAttributes::LIGHT0, GL_DIFFUSE, Color::BLACK );
		Attribs.LightColor( OpenGLAttributes::LIGHT0, GL_SPECULAR, Color::BLACK );
	} else {
		Attribs.LightColor( OpenGLAttributes::LIGHT0, GL_AMBIENT, m_SceneLight.Ambient );
		Attribs.LightColor( OpenGLAttributes::LIGHT0, GL_DIFFUSE, m_SceneLight.Diffuse );
		Attribs.LightColor( OpenGLAttributes::LIGHT0, GL_SPECULAR, m_SceneLight.Specular );
	}
	/*
	float LightParams[4];
	LightParams[0] = 0;
	LightParams[1] = 0;
	LightParams[2] = 0;
	LightParams[3] = 1.0;
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, LightParams );
	*/
}

}}
