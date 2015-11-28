#ifndef INCLUDED_RENDERER_OPENGLCONTEXTSTORE
#define INCLUDED_RENDERER_OPENGLCONTEXTSTORE

#include <vector>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

typedef unsigned long CallListHandle;

/// @brief OpenGL Context Store
///
/// OpenGL will destroy its context when the screen is resized, or during
///	other random times.  When this happens you need to rebuild all the call
///	lists, textures, shaders, etc.  Since this is a simplified Engine, I only
///	hold call lists, but in theory there could be more.
class OpenGLContextStore
{
public:
    /// @brief Constructor
    OpenGLContextStore();

    OpenGLContextStore( const OpenGLContextStore& ) = delete;

    /// @brief Destructor
    ~OpenGLContextStore();

    OpenGLContextStore& operator=( const OpenGLContextStore& ) = delete;

    /// @brief Will rebuild the internal stores.  
    ///
    /// You must call this when the 
    ///	resolution changes, or when other context destroying actions happen
    void                Rebuild();

    /// @brief Create a number of new call lists
    /// @param Count - [in] how many lists you want to make
    /// @returns a handle to the list.  Use for subsequent calls
    CallListHandle      CreateCallList( int Count );

    /// @brief Check to see whether or not the specified list exists.
    /// @param Handle - [in] handle to the list
    /// @returns true if it does
    bool                CallListExists( CallListHandle Handle ) const;

    /// @brief Get an OpenGL handle for the specified call list.  
    ///
    /// Will throw an exception if the specified list does not exist.
    /// @param Handle - [in] handle to the list
    /// @returns an int you can pass directly to OpenGL
    unsigned int        GetCallList( CallListHandle Handle ) const;

private:
    /// @brief A simple structure to hold call list info.
    struct CallListInfo
    {
        unsigned int        GLHandle;
        int                 Count;
    };
    typedef std::vector<CallListInfo> CallListInfoVector;
    void                ClearCallLists();
    CallListInfoVector  m_CallLists;
};

}
}

#endif
