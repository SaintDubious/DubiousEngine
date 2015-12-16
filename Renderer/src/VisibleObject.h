#ifndef INCLUDED_RENDERER_VISIBLEOBJECT
#define INCLUDED_RENDERER_VISIBLEOBJECT

#include "Color.h"

#include <CoordinateSpace.h>

#include <memory>
#include <vector>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

class Model;
class ObjectRenderer;
class ShadowRenderer;
typedef std::shared_ptr<Model> ModelPtr;
typedef std::shared_ptr<ObjectRenderer> ObjectRendererPtr;
typedef std::shared_ptr<ShadowRenderer> ShadowRendererPtr;

/// @brief An object that can be seen
///
/// A VisibleObject is something that can be drawn within the scene.
///	It's a pretty broad object used to define anything that is drawable.
///	For this iteration of the engine I'm just making one kind of visible
///	object and I will let it be able to draw with shadow or shine or 
///	whatever.  
class VisibleObject
{
public:
    /// @brief Constructor.  A Model is required
    /// @param pModel - [in] the model representing this object
    /// @param pShadowModel - [in] the model that represents the shadow.
    ///		This can be an empty pointer for non-shadow casting objects
    VisibleObject( ModelPtr pModel, ModelPtr pShadowModel );

    VisibleObject( const VisibleObject& ) = delete;

    /// @brief Default destructor
    ~VisibleObject() = default;

    VisibleObject& operator=( const VisibleObject& ) = delete;
    
    /// @brief Silhouette
    ///
    /// The Silhouette structore contains information used to
    ///	find a silhouette when rendering an object's shadow
    struct Silhouette
    {
        Math::LocalPoint	Position;
        typedef std::pair<Math::LocalPoint,Math::LocalPoint> Edge;
        std::vector<Edge>	Edges;
        std::vector<Silhouette> Kids;
    };

    /// @brief Builds a Silhououette for use with shadow casting.  
    /// @param LightPos - [in] Position of the light in local coord space
    /// @param Sil - [out] The resulting Silhouette
    void				BuildSilhouette( const Math::LocalPoint &LightPos, Silhouette &Sil ) const;

    /// @brief Accessor
    Math::CoordinateSpace& CoordinateSpace() { return m_CoordinateSpace; }

    /// @brief Accessor
    ModelPtr			Model() const { return m_pModel; }

    /// @brief Accessor
    ModelPtr			ShadowModel() const { return m_pShadowModel; }

    /// @brief Renderer
    ObjectRendererPtr&  ObjectRenderer() { return m_pObjectRenderer; }

    /// @brief Shadow Renderer
    ShadowRendererPtr&  ShadowRenderer() { return m_pShadowRenderer; }

    /// @brief Accessor
    Color&				BaseColor() { return m_BaseColor; }
private:

    void				BuildSilhouette( ModelPtr pModel, const Math::LocalPoint &LightPos, Silhouette &Sil ) const;

    ModelPtr			m_pModel;
    ModelPtr			m_pShadowModel;
    Math::CoordinateSpace m_CoordinateSpace;
    ObjectRendererPtr	m_pObjectRenderer;
    ShadowRendererPtr	m_pShadowRenderer;
    Color				m_BaseColor;
};

}
}

#endif
