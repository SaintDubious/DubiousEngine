#ifndef INCLUDED_RENDERER_SIMPLE_RENDERER
#define INCLUDED_RENDERER_SIMPLE_RENDERER

#include "ObjectRenderer.h"

#include <map>
#include <memory>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

class OpenGLContextStore;
typedef std::shared_ptr<OpenGLContextStore> OpenGLContextStorePtr;

/// @brief Basic object renderer.
///
/// The SimpleObjectRenderer is a very basic renderer that
///	just draws objects as a collection of flat surfaces.  No
///	fancy effects are used.
class SimpleObjectRenderer
	: public ObjectRenderer
{
public:
	/// @brief Constructor
	/// @param pContextStore - [in] the openGL context store.  This
	///		will be used to create call lists
	SimpleObjectRenderer( OpenGLContextStorePtr pContextStore );

    SimpleObjectRenderer( const SimpleObjectRenderer& ) = delete;

	/// @brief Destructor
	~SimpleObjectRenderer() = default;

    SimpleObjectRenderer& operator=( const SimpleObjectRenderer& ) = delete;
    
    /// @brief RenderObject is called to actually render the object.
	///
	///	It will render it as a collection of flat surfaces
	/// @param pObject - [in] the object to render
	virtual void        RenderObject( VisibleObjectPtr pObject );

private:
	void                ConstructModel( VisibleObjectPtr pObject );

	OpenGLContextStorePtr m_pContextStore;
	typedef std::map<VisibleObjectPtr,unsigned int> ObjectDataMap;
	ObjectDataMap       m_ObjectDataMap;
};

}
}

#endif
