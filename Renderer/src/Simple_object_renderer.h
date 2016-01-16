#ifndef INCLUDED_RENDERER_SIMPLE_RENDERER
#define INCLUDED_RENDERER_SIMPLE_RENDERER

#include "Object_renderer.h"

#include <map>
#include <memory>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

class Open_gl_context_store;

/// @brief Basic object renderer.
///
/// The SimpleObjectRenderer is a very basic renderer that
///	just draws objects as a collection of flat surfaces.  No
///	fancy effects are used.
class Simple_object_renderer
    : public Object_renderer {
public:
    /// @brief Constructor
    /// @param pContextStore - [in] the openGL context store.  This
    ///		will be used to create call lists
    Simple_object_renderer( const std::shared_ptr<Open_gl_context_store>& context_store );

    Simple_object_renderer( const Simple_object_renderer& ) = delete;

    Simple_object_renderer& operator=( const Simple_object_renderer& ) = delete;
    
    /// @brief RenderObject is called to actually render the object.
    ///
    ///	It will render it as a collection of flat surfaces
    /// @param pObject - [in] the object to render
    virtual void        render( const std::shared_ptr<Visible_object>& object );

private:
    void                construct_model( const std::shared_ptr<Visible_object>& object );

    std::shared_ptr<Open_gl_context_store> m_context_store;
    typedef std::map<std::shared_ptr<Visible_object>,unsigned int> Object_data_map;
    Object_data_map     m_object_data_map;
};

}
}

#endif
