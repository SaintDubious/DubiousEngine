#ifndef INCLUDED_RENDERER_VISIBLEOBJECT
#define INCLUDED_RENDERER_VISIBLEOBJECT

#include "Color.h"

#include <Coordinate_space.h>

#include <memory>
#include <vector>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Renderer {

class Visible_model;
class Object_renderer;
class Shadow_renderer;

/// @brief An object that can be seen
///
/// A VisibleObject is something that can be drawn within the scene.
///	It's a pretty broad object used to define anything that is drawable.
///	For this iteration of the engine I'm just making one kind of visible
///	object and I will let it be able to draw with shadow or shine or 
///	whatever.  
class Visible_object {
public:
    /// @brief Constructor.  A Model is required
    /// @param pModel - [in] the model representing this object
    /// @param pShadowModel - [in] the model that represents the shadow.
    ///		This can be an empty pointer for non-shadow casting objects
    Visible_object( const std::shared_ptr<Visible_model>& model, const std::shared_ptr<Visible_model>& shadow_model );

    Visible_object( const Visible_object& ) = delete;

    Visible_object& operator=( const Visible_object& ) = delete;
    
    /// @brief Silhouette
    ///
    /// The Silhouette structore contains information used to
    ///	find a silhouette when rendering an object's shadow
    struct Silhouette
    {
        Math::Local_point	position;
        typedef std::pair<Math::Local_point,Math::Local_point> Edge;
        std::vector<Edge>	edges;
        std::vector<Silhouette> kids;
    };

    /// @brief Builds a Silhououette for use with shadow casting.  
    /// @param LightPos - [in] Position of the light in local coord space
    /// @param Sil - [out] The resulting Silhouette
    void				build_silhouette( const Math::Local_point &light_post, Silhouette &sil ) const;

    /// @brief Accessor
    Math::Coordinate_space& coordinate_space() { return m_coordinate_space; }

    /// @brief Accessor
    std::shared_ptr<Visible_model> model() const { return m_model; }

    /// @brief Accessor
    std::shared_ptr<Visible_model> shadow_model() const { return m_shadow_model; }

    /// @brief Renderer
    std::shared_ptr<Object_renderer>& renderer() { return m_renderer; }

    /// @brief Shadow Renderer
    std::shared_ptr<Shadow_renderer>& shadow_renderer() { return m_shadow_renderer; }

    /// @brief Accessor
    Color&				base_color() { return m_base_color; }
private:

    std::shared_ptr<Visible_model>		m_model;
    std::shared_ptr<Visible_model>		m_shadow_model;
    Math::Coordinate_space m_coordinate_space;
    std::shared_ptr<Object_renderer>	m_renderer;
    std::shared_ptr<Shadow_renderer>	m_shadow_renderer;
    Color				m_base_color;
};

}
}

#endif
