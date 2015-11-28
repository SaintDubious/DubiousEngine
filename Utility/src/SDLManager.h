#ifndef INCLUDED_UTILITY_SDLMANAGER
#define INCLUDED_UTILITY_SDLMANAGER

#include <SDL.h>

#include <string>
#include <functional>
#include <utility>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Utility {

/// @brief Top Level SDL Manager
///
/// Collection of top level functions for any application.  Use this class to
///	create your main window, setup your OpenGL, and respond to system events.
///	Any application using this engine will need to make use of this object.  Events
///	are implemented as function pointers.  Set each one to a function and it will
///	be called when the event triggers.  The OnIdle event is of particular importance
///		as it is a good time to run your physics and redraw your OpenGL scene.
class SDLManager
{
public:
    /// @brief Constructor
    SDLManager();

    SDLManager( const SDLManager& ) = delete;

    /// @brief Destructor
    ~SDLManager();

    SDLManager& operator=( const SDLManager& ) = delete;

    /// @brief Creates a root window of the specified dimensions.  
    ///
    /// Probably the first thing you will call.
    ///	@param Title - [in] the window title
    /// @param Width - [in] window width, in pixels
    /// @param Height - [in] window height, in pixels
    /// @param FullScreen - [in] whether or not you want a full screen of a window
    void                CreateRootWindow( const std::string& Title, int Width, int Height, bool FullScreen );

    /// @brief Main function for running a program
    ///
    /// Call this function to begin the main application loop and fire events.  The
    ///	program will not return from this function until Stop is called (or the user closes
    ///	the main window.
    void                Run();

    /// @brief Stops the application.
    ///
    /// Causes the OnQuit function to be called and the Run function to return.  Often a 
    ///	good idea to call this when a Quit key or Quit menu option is pressed.
    void                Stop();

    /// @brief accessor to the onQuit event.  
    ///
    /// Set this to whatever event you want to fire	when the system is quitting
    std::function<void ()>& OnQuit() { return m_OnQuit; }

    /// @brief Called by the system during idle time.  
    ///
    /// This is called when all of the mouse
    ///	events have been handled and there is nothing else to do.  It will only be called
    ///	a certain amount of times per second.  Draw your scene and run your physics
    std::function<void ()>& OnIdle() { return m_OnIdle; }

    typedef std::pair<int,int> MousePoint;

    /// @brief called when the mouse is moved.
    /// @param Point - [in] The location of the mouse relative to the application window.
    std::function<void (const MousePoint&)>& OnMouseMotion() { return m_OnMouseMotion; }

    /// @brief called when the left mouse button is pressed
    /// @param Point - [in] the location of the mouse relative to the application window.
    std::function<void (const MousePoint&)>& OnMouseLeftDown() { return m_OnMouseLeftDown; }

    /// @brief called when the left mouse button is released
    /// @param Point - [in] the location of the mouse relative to the application window.
    std::function<void (const MousePoint&)>& OnMouseLeftUp() { return m_OnMouseLeftUp; }

    /// @brief called when the right mouse button is pressed
    /// @param Point - [in] the location of the mouse relative to the application window.
    std::function<void (const MousePoint&)>& OnMouseRightDown() { return m_OnMouseRightDown; }

    /// @brief called when the right mouse button is released
    /// @param Point - [in] the location of the mouse relative to the application window.
    std::function<void (const MousePoint&)>& OnMouseRightUp() { return m_OnMouseRightUp; }

    /// @brief called when the scroll wheel is moved
    /// @param Point - [in] the location of the mouse relative to the application window.
    /// @param WheelUp - [in] true if the wheel is scrolled up
    std::function<void (const MousePoint&,bool)>& OnMouseWheel() { return m_OnMouseWheel; }

    /// @brief called when a key is pressed.  
    ///
    /// See the SDL docs for specifics on the meanings of the arguments
    /// @param key - [in] the key that was pressed
    /// @param mod - [in] information about other keys that may be down
    std::function<void (SDL_Keycode,unsigned short)>& OnKeyDown() { return m_OnKeyDown; }

private:
    SDL_Window*         m_pWindow;
    bool                m_Quit;
    std::function<void ()> m_OnQuit;
    std::function<void ()> m_OnIdle;
    std::function<void (const MousePoint&)> m_OnMouseMotion;
    std::function<void (const MousePoint&)> m_OnMouseLeftDown;
    std::function<void (const MousePoint&)> m_OnMouseLeftUp;
    std::function<void (const MousePoint&)> m_OnMouseRightDown;
    std::function<void (const MousePoint&)> m_OnMouseRightUp;
    std::function<void (const MousePoint&, bool)> m_OnMouseWheel;
    std::function<void (SDL_Keycode,unsigned short)> m_OnKeyDown;
};

}
}

#endif
