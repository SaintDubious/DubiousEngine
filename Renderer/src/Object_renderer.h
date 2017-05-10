#ifndef INCLUDED_RENDERER_OBJECT_RENDERER
#define INCLUDED_RENDERER_OBJECT_RENDERER

#include <memory>

namespace Dubious {
namespace Renderer {

class Visible_object;

/// @brief Interface for a class that can render an object
///
/// The ObjectRenderer is an interface for classes that can
///	render a VisibleObject.  An object that wants to have a
///	visual representation in the 3D world will have some kind
///	of renderer attached to them.  When the Scene is drawing
///	the world it will get the attached renderer and call
///	RenderObject.
class Object_renderer {
public:
    Object_renderer( const Object_renderer& ) = delete;

    Object_renderer& operator=( const Object_renderer& ) = delete;
    
    /// @brief Renders an object
    ///
    /// The function that is called to render the visible object.
    ///	This is a pure virtual, inherited classes will implement
    ///	it differently to acheive different looks.
    ///	@param pObject - [in] the objet to be rendered.
    virtual void		render( const std::shared_ptr<Visible_object>& object ) = 0;

protected:
    Object_renderer() = default;
};

}
}

#endif