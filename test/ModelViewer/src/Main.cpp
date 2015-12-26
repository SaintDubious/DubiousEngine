#define SDL_MAIN_HANDLED
#include <SDLManager.h>
#include <AC3DFileReader.h>
#include <FilePath.h>
#include <Point.h>
#include <Scene.h>
#include <Camera.h>
#include <VisibleModel.h>
#include <VisibleObject.h>
#include <SimpleObjectRenderer.h>
#include <ShadowRenderer.h>
#include <OpenGLContextStore.h>
#include <VectorMath.h>

#include <memory>
#include <iostream>

using namespace Dubious;
namespace Dubious 
{
namespace Utility
{
typedef std::shared_ptr<AC3DModel> AC3DModelPtr;
}
namespace Renderer
{
typedef std::shared_ptr<Scene> ScenePtr;
typedef std::shared_ptr<Camera> CameraPtr;
typedef std::shared_ptr<VisibleModel> VisibleModelPtr;
typedef std::shared_ptr<VisibleObject> VisibleObjectPtr;
typedef std::shared_ptr<SimpleObjectRenderer> SimpleObjectRendererPtr;
}
}

//////////////////////////////////////////////////////////////
const float LIGHT_HEIGHT = 50.0f;
const float PI = 3.1415926535f;
const int WIDTH=800;
const int HEIGHT=600;

//////////////////////////////////////////////////////////////
// Events
void OnQuit();
void OnIdle();
void OnMouseMotion( const Utility::SDLManager::MousePoint& P );
void OnMouseLeftDown( const Utility::SDLManager::MousePoint& P );
void OnMouseLeftUp( const Utility::SDLManager::MousePoint& P );
void OnMouseWheel( int Y );
void OnKeyDown( SDL_Keycode Key, unsigned short Mod );

//////////////////////////////////////////////////////////////
// Globals
Utility::SDLManager		SDL;
Renderer::ScenePtr		pScene;
Renderer::CameraPtr		pCamera;
Utility::SDLManager::MousePoint LeftDownPoint;
bool					LeftButtonDown;
Renderer::VisibleObjectPtr pObject;

//////////////////////////////////////////////////////////////
int main( int argc, char** argv )
{
    if( argc != 3 ){
        std::cout << "Usage: TestRenderer ModelFile.ac ShadowModel.ac\n";
        return 0;
    }
    std::string VisualModelFile = argv[1];
    std::string ShadowModelFile = argv[2];

    try {
        std::cout << "Starting test\n";
        SDL.CreateRootWindow( "Model Viewer", WIDTH, HEIGHT, false );

        Utility::AC3DFilePtr pFloorFile = Utility::AC3DFileReader::ReadFile( Utility::FilePath( "Floor.ac" ) );
        Utility::AC3DFilePtr pModelFile = Utility::AC3DFileReader::ReadFile( Utility::FilePath( VisualModelFile ) );
        Utility::AC3DFilePtr pShadowFile = Utility::AC3DFileReader::ReadFile( Utility::FilePath( ShadowModelFile ) );

        Renderer::VisibleModelPtr pFloorModel( new Renderer::VisibleModel( *pFloorFile, false ) );
        Renderer::VisibleModelPtr pModel( new Renderer::VisibleModel( *pModelFile, false ) );
        Renderer::VisibleModelPtr pShadowModel( new Renderer::VisibleModel( *pShadowFile, true ) );

        Renderer::VisibleObjectPtr pFloorObject( new Renderer::VisibleObject(pFloorModel, Renderer::VisibleModelPtr()) );
        pObject = Renderer::VisibleObjectPtr( new Renderer::VisibleObject(pModel, pShadowModel) );

        pFloorObject->CoordinateSpace().Translate( Math::Vector( 0, -5, 0 ) );
        pObject->CoordinateSpace().Translate( Math::Vector( 0, 0, 0 ) );

        Renderer::OpenGLContextStorePtr pContextStore( new Renderer::OpenGLContextStore );
        pScene = Renderer::ScenePtr( new Renderer::Scene(pContextStore) );
        Renderer::SimpleObjectRendererPtr SimpleRenderer( new Renderer::SimpleObjectRenderer( pScene->ContextStore() ) );
        Renderer::ShadowRendererPtr ShadowRenderer( new Renderer::ShadowRenderer() );
        pFloorObject->ObjectRenderer() = SimpleRenderer;
        pObject->ObjectRenderer() = SimpleRenderer;
        pObject->ShadowRenderer() = ShadowRenderer;

        pScene->SceneLight().Position = Math::Point( 20, 20, 0 );
        pScene->SceneLight().Ambient = Renderer::Color( 0.2f, 0.2f, 0.2f, 1.0f );
        pScene->SceneLight().Diffuse = Renderer::Color( 0.5f, 0.5f, 0.5f, 1.0f );
        pScene->SceneLight().Specular = Renderer::Color( 0.1f, 0.1f, 0.1f, 1.0f );
        pScene->AddObject( pFloorObject );
        pScene->AddObject( pObject );

        pCamera = Renderer::CameraPtr( new Renderer::Camera( 0, 0, WIDTH,HEIGHT, 80.0f ) );
        pCamera->ZAxisOffset() = 20;
        

        SDL.OnQuit() = OnQuit;
        SDL.OnIdle() = OnIdle;
        SDL.OnMouseMotion() = OnMouseMotion;
        SDL.OnMouseLeftDown() = OnMouseLeftDown;
        SDL.OnMouseLeftUp() = OnMouseLeftUp;
        SDL.OnMouseWheel() = OnMouseWheel;
        SDL.OnKeyDown() = OnKeyDown;
        SDL.Run();

        std::cout << "Ending Normally\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cout << "Caught top level exception: " << e.what() << "\n";
    }
    catch (...) {
        std::cout << "Caught top level exception\n";
    }
    return -1;
}

//////////////////////////////////////////////////////////////
void OnQuit()
{
}

//////////////////////////////////////////////////////////////
void OnIdle()
{
	Math::Quaternion Q( Math::Vector( 0, 1.0f, 0 ), Math::ToRadians( 1.0f ) );
	pObject->CoordinateSpace().Rotate( Q );
    pScene->Render( pCamera );

    // no reason to push the CPU to 100%
    SDL_Delay( 10 );
}

//////////////////////////////////////////////////////////////
void OnMouseMotion( const Utility::SDLManager::MousePoint& P )
{
    if( LeftButtonDown ){
        Utility::SDLManager::MousePoint Offset = std::make_pair( LeftDownPoint.first - P.first, LeftDownPoint.second - P.second );
        Math::Quaternion YRotation( Math::Vector( 0, 1.0f, 0 ), Math::ToRadians(static_cast<float>(Offset.first)) ); 
        pCamera->CoordinateSpace().Rotate( YRotation );

        // Rotation around the local XAxis is limited to about 80 degrees in
        // either direction.
        Math::Vector MaxY( 0, pCamera->ZAxisOffset(), 0 );
        Math::UnitVector X, Y, Z;
        pCamera->CoordinateSpace().GetAxes( X, Y, Z );
        Math::Vector VZ( Z );
        VZ = VZ * pCamera->ZAxisOffset();
        if (Offset.second > 0) {
            if (Math::DotProduct( MaxY, VZ ) > -(pCamera->ZAxisOffset()*pCamera->ZAxisOffset()) * 0.9f) {
                Math::LocalQuaternion XRotation( Math::LocalVector( 1.0f, 0, 0 ), Math::ToRadians(static_cast<float>(Offset.second)) );
                pCamera->CoordinateSpace().Rotate( XRotation );
            }
        }
        else if (Offset.second < 0) {
            if (Math::DotProduct( MaxY, VZ ) < (pCamera->ZAxisOffset()*pCamera->ZAxisOffset()) * 0.9f) {
                Math::LocalQuaternion XRotation( Math::LocalVector( 1.0f, 0, 0 ), Math::ToRadians(static_cast<float>(Offset.second)) );
                pCamera->CoordinateSpace().Rotate( XRotation );
            }
        }

        LeftDownPoint = P;
    }
}

//////////////////////////////////////////////////////////////
void OnMouseLeftDown( const Utility::SDLManager::MousePoint& P )
{
    LeftButtonDown = true;
    LeftDownPoint = P;
}

//////////////////////////////////////////////////////////////
void OnMouseLeftUp( const Utility::SDLManager::MousePoint&  )
{
    LeftButtonDown = false;
}

//////////////////////////////////////////////////////////////
void OnMouseWheel( int Y )
{
    int Delta = Y*-5;
    if( pCamera->ZAxisOffset() + Delta < 5 ) {
        return;
    }
    pCamera->ZAxisOffset() += Delta;
}

//////////////////////////////////////////////////////////////
void OnKeyDown( SDL_Keycode Key, unsigned short Mod )
{
    switch( Key )
    {
    case SDLK_q:
        SDL.Stop();
        break;
    }
}

