#ifndef INCLUDED_RENDERER_SHADOWRENDERER
#define INCLUDED_RENDERER_SHADOWRENDERER

#include "Visible_object.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

/// @brief An object that can render a shadow volume
///
/// A ShadowRenderer is an object capable of rendering a shadow.  The
///	Scene object will call on this while rendering the scene to draw
///	the shadow of each object
class Shadow_renderer {
public:
    /// @brief Constructor
    Shadow_renderer() = default;

    Shadow_renderer( const Shadow_renderer& ) = delete;

    Shadow_renderer& operator=( const Shadow_renderer& ) = delete;

    /// @brief Call to render the shadow
    /// @param pVisibleObject - [in] the object that needs a shadow
    /// @param LighPosition - [in] Where the light is in the local coord space
    void                render_shadow_volume( const std::shared_ptr<Visible_object>& object, const Math::Local_point& ligh_position );
};

}
}

#endif
