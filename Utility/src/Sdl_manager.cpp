#include "Sdl_manager.h"
#include "Timer.h"

#include <SDL_opengl.h>
#include <stdexcept>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Utility {

namespace
{
static const int LEFT_MOUSE_BUTTON		= 1;
static const int RIGHT_MOUSE_BUTTON		= 3;
static const int MOUSE_SCROLL_UP		= 4;
static const int MOUSE_SCROLL_DOWN		= 5;
}

//////////////////////////////////////////////////////////////
Sdl_manager::Sdl_manager()
{
    if (SDL_Init( SDL_INIT_VIDEO ) < 0) {
        throw std::runtime_error( "Could not initialize SDL" );
    }
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
//	SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 1 );
    SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8 );
}

//////////////////////////////////////////////////////////////
Sdl_manager::~Sdl_manager()
{
    if (m_gl_context != nullptr) {
        SDL_GL_DeleteContext( m_gl_context );
    }
    if (m_window != nullptr) {
        SDL_DestroyWindow( m_window );
    }
    SDL_Quit();
}

//////////////////////////////////////////////////////////////
void Sdl_manager::create_root_window( const std::string& title, int width, int height, bool full_screen )
{
    int mode = SDL_WINDOW_OPENGL;
    if (full_screen) {
        mode |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    m_window = SDL_CreateWindow( title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, mode );
    if (m_window == nullptr) {
        throw std::runtime_error( "Could not create root window" );
    }
    m_gl_context = SDL_GL_CreateContext( m_window );
    if (m_gl_context == nullptr) {
        throw std::runtime_error( "Failed to create OpenGL Context" );
    }
}

//////////////////////////////////////////////////////////////
void Sdl_manager::run()
{
    Timer loop_timer;
    loop_timer.start();
    m_quit = false;
    while (!m_quit) {
        // Start by polling an existing SDL events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                m_quit = true;
                if (m_on_quit) {
                    m_on_quit();
                }
                break;
            case SDL_KEYDOWN:
                if (m_on_key_down) {
                    m_on_key_down( event.key.keysym.sym, event.key.keysym.mod );
                }
                break;
            case SDL_MOUSEMOTION:
                if (m_on_mouse_motion) {
                    m_on_mouse_motion( std::make_pair(event.motion.x, event.motion.y) );
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == LEFT_MOUSE_BUTTON && m_on_mouse_left_down) {
                    m_on_mouse_left_down( std::make_pair(event.motion.x, event.motion.y) );
                } 
                else if (event.button.button == RIGHT_MOUSE_BUTTON && m_on_mouse_right_down) {
                    m_on_mouse_right_down( std::make_pair(event.motion.x, event.motion.y) );
                } 
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == LEFT_MOUSE_BUTTON && m_on_mouse_left_up ) {
                    m_on_mouse_left_up( std::make_pair(event.motion.x, event.motion.y) );
                } 
                else if (event.button.button == RIGHT_MOUSE_BUTTON && m_on_mouse_right_up) {
                    m_on_mouse_right_up( std::make_pair(event.motion.x, event.motion.y) );
                }
                break;
            case SDL_MOUSEWHEEL:
                if (m_on_mouse_wheel) {
                    m_on_mouse_wheel( event.wheel.y );
                }
                break;
            }
        }
        // If there are no SDL events then check to see if the user wants to quit
        if (m_quit) {
            break;
        }
        if (loop_timer.elapsed() > 5) {
            loop_timer.restart();
            // We have some free cycles, let the user decide what to do
            if (m_on_idle) {
                m_on_idle();
            }
            glFinish();
            SDL_GL_SwapWindow( m_window );
        }
    }
}

//////////////////////////////////////////////////////////////
void Sdl_manager::stop()
{
    m_quit = true;
}

}}
