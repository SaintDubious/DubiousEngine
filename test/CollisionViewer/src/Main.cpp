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
#include <OpenGLContextStore.h>
#include <VectorMath.h>
#include <Timer.h>
#include <CollisionSolver.h>
#include <PhysicsObject.h>
#include <PhysicsModel.h>

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
    namespace Physics
    {
        typedef std::shared_ptr<PhysicsModel> PhysicsModelPtr;
        typedef std::shared_ptr<PhysicsObject> PhysicsObjectPtr;
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
Utility::SDLManager		    SDL;
Renderer::ScenePtr		    pScene;
Renderer::CameraPtr		    pCamera;
Utility::SDLManager::MousePoint LeftDownPoint;
bool					    LeftButtonDown;
Renderer::SimpleObjectRendererPtr SimpleRenderer;

Physics::CollisionSolver    Solver;
Renderer::VisibleObjectPtr  pVisibleObject1;
Physics::PhysicsObjectPtr   pPhysicsObject1;
Renderer::VisibleObjectPtr  pVisibleObject2;
Physics::PhysicsObjectPtr   pPhysicsObject2;
Renderer::VisibleObjectPtr  SelectedVisibleObject;
Physics::PhysicsObjectPtr   SelectedPhysicsObject;

//////////////////////////////////////////////////////////////
int main( int argc, char** argv )
{
    try {
        std::cout << "Starting test\n";
        SDL.CreateRootWindow( "Collision Viewer", WIDTH, HEIGHT, false );


        Renderer::VisibleModelPtr pVisibleModel( new Renderer::VisibleModel( Math::Triple( 1, 1, 1 ), false ) );
        Physics::PhysicsModelPtr pPhysicsModel( new Physics::PhysicsModel( Math::Triple( 1, 1, 1 ) ) );

        pVisibleObject1 = Renderer::VisibleObjectPtr( new Renderer::VisibleObject(pVisibleModel, pVisibleModel) );
        pVisibleObject1->CoordinateSpace().Translate( Math::Vector( 0, 0, 0 ) );
        pPhysicsObject1 = Physics::PhysicsObjectPtr( new Physics::PhysicsObject( pPhysicsModel ) );
        pPhysicsObject1->CoordinateSpace().Translate( Math::Vector( 0, 0, 0 ) );

        pVisibleObject2 = Renderer::VisibleObjectPtr( new Renderer::VisibleObject(pVisibleModel, pVisibleModel) );
        pVisibleObject2->CoordinateSpace().Translate( Math::Vector( 3, 0, 0 ) );
        pPhysicsObject2 = Physics::PhysicsObjectPtr( new Physics::PhysicsObject( pPhysicsModel ) );
        pPhysicsObject2->CoordinateSpace().Translate( Math::Vector( 3, 0, 0 ) );

        SelectedVisibleObject = pVisibleObject1;
        SelectedPhysicsObject = pPhysicsObject1;

        Renderer::OpenGLContextStorePtr pContextStore( new Renderer::OpenGLContextStore );
        pScene = Renderer::ScenePtr( new Renderer::Scene(pContextStore) );
        SimpleRenderer.reset( new Renderer::SimpleObjectRenderer( pScene->ContextStore() ) );

        pVisibleObject1->ObjectRenderer() = SimpleRenderer;
        pVisibleObject2->ObjectRenderer() = SimpleRenderer;

        pScene->SceneLight().Position = Math::Point( 20, 20, 0 );
        pScene->SceneLight().Ambient = Renderer::Color( 0.2f, 0.2f, 0.2f, 1.0f );
        pScene->SceneLight().Diffuse = Renderer::Color( 0.5f, 0.5f, 0.5f, 1.0f );
        pScene->SceneLight().Specular = Renderer::Color( 0.1f, 0.1f, 0.1f, 1.0f );

        pScene->AddObject( pVisibleObject1 );
        pScene->AddObject( pVisibleObject2 );

        pCamera = Renderer::CameraPtr( new Renderer::Camera( 0, 0, WIDTH,HEIGHT, 80.0f ) );
        pCamera->ZAxisOffset() = 20;

        SDL.OnQuit() = OnQuit;
        SDL.OnIdle() = OnIdle;
        SDL.OnMouseMotion() = OnMouseMotion;
        SDL.OnMouseLeftDown() = OnMouseLeftDown;
        SDL.OnMouseLeftUp() = OnMouseLeftUp;
        SDL.OnMouseWheel() = OnMouseWheel;
        SDL.OnKeyDown() = OnKeyDown;

        std::cout << "Object 1 Selected\n";
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
//    Math::Quaternion Q( Math::Vector( 0, 1.0f, 0 ), Math::ToRadians( 1.0f ) );
//    pObject->CoordinateSpace().Rotate( Q );
    pScene->Render( *pCamera );

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
    case SDLK_1:
        SelectedVisibleObject = pVisibleObject1;
        SelectedPhysicsObject = pPhysicsObject1;
        std::cout << "Object 1 Selected\n";
        break;
    case SDLK_2:
        SelectedVisibleObject = pVisibleObject2;
        SelectedPhysicsObject = pPhysicsObject2;
        std::cout << "Object 2 Selected\n";
        break;
    case SDLK_t:
        {
            std::cout << "Object 1: " << pPhysicsObject1->CoordinateSpace() << "\n"
                      << "Object 2: " << pPhysicsObject2->CoordinateSpace() << "\n";
            bool Intersection = Solver.Intersection( *pPhysicsObject1, *pPhysicsObject2 );
            if (Intersection) {
                std::cout << "Objects intersect\n";
            }
            else {
                std::cout << "Objects do not intersect\n";
            }
        }
        break;
    case SDLK_w:
        {
            Math::Vector D( 0, 0, -1 );
            SelectedVisibleObject->CoordinateSpace().Translate( D );
            SelectedPhysicsObject->CoordinateSpace().Translate( D );
        }
        break;
    case SDLK_a:
        {
            Math::Vector D( -1, 0, 0 );
            SelectedVisibleObject->CoordinateSpace().Translate( D );
            SelectedPhysicsObject->CoordinateSpace().Translate( D );
        }
        break;
    case SDLK_s:
        {
            Math::Vector D( 0, 0, 1 );
            SelectedVisibleObject->CoordinateSpace().Translate( D );
            SelectedPhysicsObject->CoordinateSpace().Translate( D );
        }
        break;
    case SDLK_d:
        {
            Math::Vector D( 1, 0, 0 );
            SelectedVisibleObject->CoordinateSpace().Translate( D );
            SelectedPhysicsObject->CoordinateSpace().Translate( D );
        }
        break;
    case SDLK_z:
        {
            Math::Vector D( 0, 1, 0 );
            SelectedVisibleObject->CoordinateSpace().Translate( D );
            SelectedPhysicsObject->CoordinateSpace().Translate( D );
        }
        break;
    case SDLK_x:
        {
            Math::Vector D( 0, -1, 0 );
            SelectedVisibleObject->CoordinateSpace().Translate( D );
            SelectedPhysicsObject->CoordinateSpace().Translate( D );
        }
        break;
    case SDLK_r:
        {
            srand( static_cast<unsigned long>(time(NULL)) );
            Math::Quaternion Q( Math::Vector( static_cast<float>(rand())/static_cast<float>(RAND_MAX), 
                                              static_cast<float>(rand())/static_cast<float>(RAND_MAX), 
                                              static_cast<float>(rand())/static_cast<float>(RAND_MAX) ), 
                                              Math::ToRadians(360.0f*static_cast<float>(rand())/static_cast<float>(RAND_MAX)) );
            SelectedVisibleObject->CoordinateSpace().Rotate( Q );
            SelectedPhysicsObject->CoordinateSpace().Rotate( Q );
        }
        break;
    }
}

