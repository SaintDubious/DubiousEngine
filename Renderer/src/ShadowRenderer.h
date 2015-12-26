#ifndef INCLUDED_RENDERER_SHADOWRENDERER
#define INCLUDED_RENDERER_SHADOWRENDERER

#include "VisibleObject.h"

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

class VisibleObject;
typedef std::shared_ptr<VisibleObject> VisibleObjectPtr;

/// @brief An object that can render a shadow volume
///
/// A ShadowRenderer is an object capable of rendering a shadow.  The
///	Scene object will call on this while rendering the scene to draw
///	the shadow of each object
class ShadowRenderer
{
public:
    /// @brief Constructor
    ShadowRenderer() = default;

    ShadowRenderer( const ShadowRenderer& ) = delete;

    /// @brief Destructor
    ~ShadowRenderer() = default;

    ShadowRenderer& operator=( const ShadowRenderer& ) = delete;

    /// @brief Call to render the shadow
    /// @param pVisibleObject - [in] the object that needs a shadow
    /// @param LighPosition - [in] Where the light is in the local coord space
    void                RenderShadowVolume( VisibleObjectPtr pVisibleObject, const Math::LocalPoint& LighPosition );
};

}
}

#endif
