#include "OpenGLContextStore.h"
#include <SDL_opengl.h>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

//////////////////////////////////////////////////////////////
OpenGLContextStore::OpenGLContextStore()
{
}

//////////////////////////////////////////////////////////////
OpenGLContextStore::~OpenGLContextStore()
{
    ClearCallLists();
}

//////////////////////////////////////////////////////////////
void OpenGLContextStore::Rebuild()
{
    ClearCallLists();
}

//////////////////////////////////////////////////////////////
CallListHandle OpenGLContextStore::CreateCallList( int Count )
{
    CallListInfo NewInfo;
    NewInfo.Count = Count;
    NewInfo.GLHandle = glGenLists( Count );
    m_CallLists.push_back( NewInfo );
    return static_cast<CallListHandle>( m_CallLists.size()-1 );
}

//////////////////////////////////////////////////////////////
void OpenGLContextStore::ClearCallLists()
{
    for (size_t i=0; i<m_CallLists.size(); ++i) {
        glDeleteLists( m_CallLists[i].GLHandle, m_CallLists[i].Count );
    }
    m_CallLists.clear();
}

//////////////////////////////////////////////////////////////
bool OpenGLContextStore::CallListExists( CallListHandle Handle ) const
{
    return (Handle<m_CallLists.size());
}

//////////////////////////////////////////////////////////////
unsigned int OpenGLContextStore::GetCallList( CallListHandle Handle ) const
{
    if( !CallListExists( Handle ) )
        throw std::runtime_error( "Specified call list handle doesn't exist" );
    return m_CallLists[Handle].GLHandle;
}

}}
