#ifndef INCLUDED_RENDERER_OPENGLCONTEXTSTORE
#define INCLUDED_RENDERER_OPENGLCONTEXTSTORE

#include <vector>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

typedef unsigned long Call_list_handle;

/// @brief OpenGL Context Store
///
/// OpenGL will destroy its context when the screen is resized, or during
///	other random times.  When this happens you need to rebuild all the call
///	lists, textures, shaders, etc.  Since this is a simplified Engine, I only
///	hold call lists, but in theory there could be more.
class Open_gl_context_store
{
public:
    /// @brief Constructor
    Open_gl_context_store();

    Open_gl_context_store( const Open_gl_context_store& ) = delete;

    /// @brief Destructor
    ~Open_gl_context_store();

    Open_gl_context_store& operator=( const Open_gl_context_store& ) = delete;

    /// @brief Will rebuild the internal stores.  
    ///
    /// You must call this when the 
    ///	resolution changes, or when other context destroying actions happen
    void                rebuild();

    /// @brief Create a number of new call lists
    /// @param Count - [in] how many lists you want to make
    /// @returns a handle to the list.  Use for subsequent calls
    Call_list_handle    create_call_list( int count );

    /// @brief Check to see whether or not the specified list exists.
    /// @param Handle - [in] handle to the list
    /// @returns true if it does
    bool                call_list_exists( Call_list_handle handle ) const;

    /// @brief Get an OpenGL handle for the specified call list.  
    ///
    /// Will throw an exception if the specified list does not exist.
    /// @param Handle - [in] handle to the list
    /// @returns an int you can pass directly to OpenGL
    unsigned int        get_call_list( Call_list_handle handle ) const;

private:
    /// @brief A simple structure to hold call list info.
    struct Call_list_info
    {
        unsigned int    gl_handle;
        int             count;
    };
    void                clear_call_lists();
    std::vector<Call_list_info>  m_call_lists;
};

}
}

#endif
