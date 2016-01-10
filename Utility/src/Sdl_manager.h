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
class Sdl_manager {
public:
    /// @brief Constructor
    Sdl_manager();

    Sdl_manager( const Sdl_manager& ) = delete;

    /// @brief Destructor
    ~Sdl_manager();

    Sdl_manager& operator=( const Sdl_manager& ) = delete;

    /// @brief Creates a root window of the specified dimensions.  
    ///
    /// Probably the first thing you will call.
    ///	@param Title - [in] the window title
    /// @param Width - [in] window width, in pixels
    /// @param Height - [in] window height, in pixels
    /// @param FullScreen - [in] whether or not you want a full screen of a window
    void                create_root_window( const std::string& title, int width, int height, bool full_screen );

    /// @brief Main function for running a program
    ///
    /// Call this function to begin the main application loop and fire events.  The
    ///	program will not return from this function until Stop is called (or the user closes
    ///	the main window.
    void                run();

    /// @brief Stops the application.
    ///
    /// Causes the OnQuit function to be called and the Run function to return.  Often a 
    ///	good idea to call this when a Quit key or Quit menu option is pressed.
    void                stop();

    /// @brief accessor to the onQuit event.  
    ///
    /// Set this to whatever event you want to fire	when the system is quitting
    std::function<void ()>& on_quit() { return m_on_quit; }

    /// @brief Called by the system during idle time.  
    ///
    /// This is called when all of the mouse
    ///	events have been handled and there is nothing else to do.  It will only be called
    ///	a certain amount of times per second.  Draw your scene and run your physics
    std::function<void ()>& on_idle() { return m_on_idle; }

    typedef std::pair<int,int> Mouse_point;

    /// @brief called when the mouse is moved.
    /// @param Point - [in] The location of the mouse relative to the application window.
    std::function<void (const Mouse_point&)>& on_mouse_motion() { return m_on_mouse_motion; }

    /// @brief called when the left mouse button is pressed
    /// @param Point - [in] the location of the mouse relative to the application window.
    std::function<void (const Mouse_point&)>& on_mouse_left_down() { return m_on_mouse_left_down; }

    /// @brief called when the left mouse button is released
    /// @param Point - [in] the location of the mouse relative to the application window.
    std::function<void (const Mouse_point&)>& on_mouse_left_up() { return m_on_mouse_left_up; }

    /// @brief called when the right mouse button is pressed
    /// @param Point - [in] the location of the mouse relative to the application window.
    std::function<void (const Mouse_point&)>& on_mouse_right_down() { return m_on_mouse_right_down; }

    /// @brief called when the right mouse button is released
    /// @param Point - [in] the location of the mouse relative to the application window.
    std::function<void (const Mouse_point&)>& on_mouse_right_up() { return m_on_mouse_right_up; }

    /// @brief called when the scroll wheel is moved
    /// @param Point - [in] the location of the mouse relative to the application window.
    /// @param WheelUp - [in] true if the wheel is scrolled up
    std::function<void (int)>& on_mouse_Wheel() { return m_on_mouse_wheel; }

    /// @brief called when a key is pressed.  
    ///
    /// See the SDL docs for specifics on the meanings of the arguments
    /// @param key - [in] the key that was pressed
    /// @param mod - [in] information about other keys that may be down
    std::function<void (SDL_Keycode,unsigned short)>& on_key_down() { return m_on_key_down; }

private:
    SDL_Window*         m_window        = nullptr;
    SDL_GLContext       m_gl_context    = nullptr;
    bool                m_quit          = false;
    std::function<void ()> m_on_quit;
    std::function<void ()> m_on_idle;
    std::function<void (const Mouse_point&)> m_on_mouse_motion;
    std::function<void (const Mouse_point&)> m_on_mouse_left_down;
    std::function<void (const Mouse_point&)> m_on_mouse_left_up;
    std::function<void (const Mouse_point&)> m_on_mouse_right_down;
    std::function<void (const Mouse_point&)> m_on_mouse_right_up;
    std::function<void (int Y)> m_on_mouse_wheel;
    std::function<void (SDL_Keycode,unsigned short)> m_on_key_down;
};

}
}

#endif
