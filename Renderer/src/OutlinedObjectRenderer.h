#ifndef INCLUDED_RENDERER_OUTLINEDOLIDOBJECTRENDERER
#define INCLUDED_RENDERER_OUTLINEDOLIDOBJECTRENDERER

#include "ObjectRenderer.h"

#include <map>
#include <memory>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

class OpenGLContextStore;
class VisibleObject;
class VisibleModel;
typedef std::shared_ptr<OpenGLContextStore> OpenGLContextStorePtr;
typedef std::shared_ptr<VisibleObject> VisibleObjectPtr;
typedef std::shared_ptr<VisibleModel> VisibleModelPtr;

/// @brief Renders objects with an outline
///
/// This renderer will show the objects with a thick black outline
/// around the edges.  It works well when you're trying to create
/// a cel shaded look, though these days I don't bother with the full
/// cel shading.
class OutlinedObjectRenderer
    : public ObjectRenderer
{
public:
    /// @brief Constructor
    /// @param pContextStore - [in] the openGL context store.  This
    ///		will be used to create call lists
    OutlinedObjectRenderer( OpenGLContextStorePtr pContextStore );

    OutlinedObjectRenderer( const OutlinedObjectRenderer& ) = delete;

    /// @brief Default destructor
    ~OutlinedObjectRenderer() = default;

    OutlinedObjectRenderer& operator=( const OutlinedObjectRenderer& ) = delete;

    /// @brief RenderObject is called to actually render the object.
    ///
    ///	It will render it as a collection of flat surfaces with a border
    /// @param pObject - [in] the object to render
    virtual void        RenderObject( VisibleObjectPtr pVisibleObject );

private:
    void                ConstructModel( VisibleObjectPtr pVisibleObject );

    OpenGLContextStorePtr m_pContextStore;
    typedef std::map<VisibleObjectPtr,unsigned int> ObjectDataMap;
    ObjectDataMap       m_ObjectDataMap;
    ObjectDataMap       m_FlatModelMap;
};

}
}

#endif
