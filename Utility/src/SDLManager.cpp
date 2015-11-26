#include "SDLManager.h"
#include "Timer.h"

#include <SDL_opengl.h>
#include <stdexcept>

namespace
{
static const int LEFT_MOUSE_BUTTON		= 1;
static const int RIGHT_MOUSE_BUTTON		= 3;
static const int MOUSE_SCROLL_UP		= 4;
static const int MOUSE_SCROLL_DOWN		= 5;
}

//////////////////////////////////////////////////////////////
namespace Dubious
{
namespace Utility
{

//////////////////////////////////////////////////////////////
SDLManager::SDLManager()
{
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
		throw std::runtime_error( "Could not initialize SDL" );
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
//	SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 1 );
	SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
}

//////////////////////////////////////////////////////////////
SDLManager::~SDLManager()
{
    if (m_pWindow != NULL)
        SDL_DestroyWindow( m_pWindow );
	SDL_Quit();
}

//////////////////////////////////////////////////////////////
void SDLManager::CreateRootWindow( const std::string& Title, int Width, int Height, bool FullScreen )
{
	int Mode = SDL_WINDOW_OPENGL;
	if (FullScreen)
		Mode |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    m_pWindow = SDL_CreateWindow( Title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Width, Height, Mode );
    if (m_pWindow == NULL) 
        throw std::runtime_error( "Could not create root window" );
}

//////////////////////////////////////////////////////////////
void SDLManager::Run()
{
	Timer LoopTimer;
	LoopTimer.Start();
	m_Quit = false;
	while( !m_Quit ){
		// Start by polling an existing SDL events
		SDL_Event Event;
		while( SDL_PollEvent(&Event) ){
			switch( Event.type )
			{
			case SDL_QUIT:
				m_Quit = true;
				if (m_OnQuit) {
					m_OnQuit();
				}
				break;
			case SDL_KEYDOWN:
				if (m_OnKeyDown) {
					m_OnKeyDown( Event.key.keysym.sym, Event.key.keysym.mod );
				}
				break;
			case SDL_MOUSEMOTION:
				if (m_OnMouseMotion) {
					m_OnMouseMotion( std::make_pair(Event.motion.x, Event.motion.y) );
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if( Event.button.button == LEFT_MOUSE_BUTTON && m_OnMouseLeftDown){
					m_OnMouseLeftDown( std::make_pair(Event.motion.x, Event.motion.y) );
				} else if( Event.button.button == RIGHT_MOUSE_BUTTON && m_OnMouseRightDown){
					m_OnMouseRightDown( std::make_pair(Event.motion.x, Event.motion.y) );
				} else if( ((Event.button.button == MOUSE_SCROLL_UP) || (Event.button.button == MOUSE_SCROLL_DOWN)) && m_OnMouseWheel ){
					m_OnMouseWheel( std::make_pair(Event.motion.x, Event.motion.y), Event.button.button == MOUSE_SCROLL_UP );
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if( Event.button.button == LEFT_MOUSE_BUTTON && m_OnMouseLeftUp ){
					m_OnMouseLeftUp( std::make_pair(Event.motion.x, Event.motion.y) );
				} else if( Event.button.button == RIGHT_MOUSE_BUTTON && m_OnMouseRightUp){
					m_OnMouseRightUp( std::make_pair(Event.motion.x, Event.motion.y) );
				}
				break;
			}
		}
		// If there are no SDL events then check to see if the user wants to quit
		if( m_Quit )
			break;
		if( LoopTimer.Elapsed() > 5 ){
			LoopTimer.Restart();
			// We have some free cycles, let the user decide what to do
			if (m_OnIdle) {
				m_OnIdle();
			}
			glFinish();
		}
	}
}

//////////////////////////////////////////////////////////////
void SDLManager::Stop()
{
	m_Quit = true;
}

}
}
