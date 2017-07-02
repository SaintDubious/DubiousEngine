#ifndef INCLUDED_RENDERER_SCENE
#define INCLUDED_RENDERER_SCENE

#include "Color.h"

#include <Point.h>

#include <memory>
#include <list>

namespace Dubious {
namespace Renderer {

class Open_gl_context_store;
class Visible_object;
class Open_gl_attributes;
class Camera;

/// @brief A Visible Scene
///
/// A Scene represents a collection of drawable object that will
///	be drawn.  It is really the top level construct for the Renderer
///	namespace.
class Scene {
public:
    /// @brief Constructor
    /// @param store - [in] An Open GL Context Store
    Scene(const std::shared_ptr<Open_gl_context_store>& store);

    Scene(const Scene&) = delete;

    Scene& operator=(const Scene&) = delete;

    /// @brief A Light
    ///
    /// This struct is just a simple grouping of the properties
    ///	of a light.
    struct Light {
        Math::Point position;
        Color       ambient;
        Color       diffuse;
        Color       specular;
    };

    /// @brief Call this to render the scene
    void render(Camera& camera);

    /// @brief Add an object to the scene
    /// @param object - [in] the object
    void add_object(const std::shared_ptr<Visible_object>& object);

    /// @brief Remove and object from the scene
    /// @param object - [in] the object
    void remove_object(const std::shared_ptr<Visible_object>& object);

    /// @brief Remove all objects from the scene
    void remove_all_objects();

    /// @brief Accessor to the light
    Light& scene_light() { return m_scene_light; }

    /// @brief Accessor to the context store
    const Open_gl_context_store& context_store() const { return *m_context_store; }

private:
    void setup_light(Open_gl_attributes& attribs, bool in_shadow) const;

    std::shared_ptr<Open_gl_context_store>     m_context_store;
    std::list<std::shared_ptr<Visible_object>> m_objects;
    Light                                      m_scene_light;
};

}  // namespace Renderer
}  // namespace Dubious
#endif
