#ifndef INCLUDED_RENDERER_SCENE
#define INCLUDED_RENDERER_SCENE

#include "Color.h"

#include <Point.h>

#include <memory>
#include <list>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

class Camera;
class OpenGLContextStore;
class OpenGLAttributes;
class VisibleObject;
typedef std::shared_ptr<Camera> CameraPtr;
typedef std::shared_ptr<OpenGLContextStore> OpenGLContextStorePtr;
typedef std::shared_ptr<VisibleObject> VisibleObjectPtr;
typedef std::list<VisibleObjectPtr> VisibleObjectPtrList;

/// @brief A Visible Scene
///
/// A Scene represents a collection of drawable object that will
///	be drawn.  It is really the top level construct for the Renderer
///	namespace.
class Scene 
{
public:
    /// @brief Constructor
	/// @param pStore - [in] An Open GL Context Store
	Scene( OpenGLContextStorePtr pStore );

    Scene( const Scene& ) = delete;

	/// @brief Destructor
	~Scene() = default;

    Scene& operator=( const Scene& ) = delete;
    
    /// @brief A Light
	///
	/// This struct is just a simple grouping of the properties
	///	of a light.
	struct Light
	{
		Math::Point 	Position;
		Color			Ambient;
		Color			Diffuse;
		Color			Specular;
	};

	/// @brief Call this to render the scene
	void				Render( CameraPtr pCamera );

	/// @brief Add an object to the scene
	/// @param pObject - [in] the object
	void				AddObject( VisibleObjectPtr pObject );

	/// @brief Remove and object from the scene
	/// @param pObject - [in] the object
	void				RemoveObject( VisibleObjectPtr pObject );

	/// @brief Remove all objects from the scene
	void				RemoveAllObjects();

	/// @brief Accessor to the light
	Light&				SceneLight() { return m_SceneLight; }

	/// @brief Accessor to the context store
	OpenGLContextStorePtr ContextStore() const { return m_pContextStore; }

private:

	void				SetupLight( OpenGLAttributes& Attribs, bool InShadow ) const;

	OpenGLContextStorePtr m_pContextStore;
	VisibleObjectPtrList m_Objects;
	Light				m_SceneLight;
};

}
}
#endif
