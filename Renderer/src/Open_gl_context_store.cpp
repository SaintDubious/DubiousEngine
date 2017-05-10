#include "Open_gl_context_store.h"
#include <SDL_opengl.h>

namespace Dubious {
namespace Renderer {

Open_gl_context_store::Open_gl_context_store()
{
}

Open_gl_context_store::~Open_gl_context_store()
{
    clear_call_lists();
}

void Open_gl_context_store::rebuild()
{
    clear_call_lists();
}

Call_list_handle Open_gl_context_store::create_call_list( int Count )
{
    Call_list_info new_info;
    new_info.count = Count;
    new_info.gl_handle = glGenLists( Count );
    m_call_lists.push_back( new_info );
    return static_cast<Call_list_handle>( m_call_lists.size()-1 );
}

void Open_gl_context_store::clear_call_lists()
{
    for (auto list : m_call_lists) {
        glDeleteLists( list.gl_handle, list.count );
    }
    m_call_lists.clear();
}

bool Open_gl_context_store::call_list_exists( Call_list_handle handle ) const
{
    return (handle < m_call_lists.size());
}

unsigned int Open_gl_context_store::get_call_list( Call_list_handle handle ) const
{
    if( !call_list_exists( handle ) )
        throw std::runtime_error( "Specified call list handle doesn't exist" );
    return m_call_lists[handle].gl_handle;
}

}}
