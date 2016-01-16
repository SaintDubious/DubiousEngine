#ifndef INCLUDED_RENDERER_OUTLINEDOLIDOBJECTRENDERER
#define INCLUDED_RENDERER_OUTLINEDOLIDOBJECTRENDERER

#include "Object_renderer.h"

#include <map>
#include <memory>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

class Open_gl_context_store;

/// @brief Renders objects with an outline
///
/// This renderer will show the objects with a thick black outline
/// around the edges.  It works well when you're trying to create
/// a cel shaded look, though these days I don't bother with the full
/// cel shading.
class Outlined_object_renderer
    : public Object_renderer {
public:
    /// @brief Constructor
    /// @param pContextStore - [in] the openGL context store.  This
    ///		will be used to create call lists
    Outlined_object_renderer( const std::shared_ptr<Open_gl_context_store>& context_store );

    Outlined_object_renderer( const Outlined_object_renderer& ) = delete;

    Outlined_object_renderer& operator=( const Outlined_object_renderer& ) = delete;

    /// @brief RenderObject is called to actually render the object.
    ///
    ///	It will render it as a collection of flat surfaces with a border
    /// @param pObject - [in] the object to render
    virtual void        render( const std::shared_ptr<Visible_object>& object );

private:
    void                construct_model( const std::shared_ptr<Visible_object>& object );

    std::shared_ptr<Open_gl_context_store> m_context_store;
    typedef std::map<std::shared_ptr<Visible_object>,unsigned int> Object_data_map;
    Object_data_map     m_object_data_map;
    Object_data_map     m_flat_model_map;
};

}
}

#endif
