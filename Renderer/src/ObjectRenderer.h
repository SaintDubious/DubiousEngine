#ifndef INCLUDED_RENDERER_OBJECT_RENDERER
#define INCLUDED_RENDERER_OBJECT_RENDERER

#include <memory>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

class VisibleObject;
typedef std::shared_ptr<VisibleObject> VisibleObjectPtr;

/// @brief Interface for a class that can render an object
///
/// The ObjectRenderer is an interface for classes that can
///	render a VisibleObject.  An object that wants to have a
///	visual representation in the 3D world will have some kind
///	of renderer attached to them.  When the Scene is drawing
///	the world it will get the attached renderer and call
///	RenderObject.
class ObjectRenderer
{
public:
    ObjectRenderer( const ObjectRenderer& ) = delete;

    /// @brief Default Destructor
    virtual ~ObjectRenderer() = default;

    ObjectRenderer& operator=( const ObjectRenderer& ) = delete;
    
    /// @brief Renders an object
    ///
    /// The function that is called to render the visible object.
    ///	This is a pure virtual, inherited classes will implement
    ///	it differently to acheive different looks.
    ///	@param pObject - [in] the objet to be rendered.
    virtual void		RenderObject( VisibleObjectPtr pObject ) = 0;

protected:
    ObjectRenderer() = default;

};

}
}

#endif