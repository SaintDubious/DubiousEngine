#ifndef INCLUDED_RENDERER_VISIBLEMODEL
#define INCLUDED_RENDERER_VISIBLEMODEL

#include "Color.h"

#include <Point.h>
#include <Unit_vector.h>

#include <vector>
#include <memory>

namespace RendererTest {
class Visible_model_test;
}

namespace Dubious {

namespace Utility {
class Ac3d_file;
class Ac3d_model;
class Ac3d_material;
}  // namespace Utility

namespace Renderer {

/// @brief Representation of a visible model
///
/// The Renderer::Visible_model is a model suitable for drawing in
///	OpenGL.  It is similar to the Ac3d_model, but it contains
///	more information, like point normals.
class Visible_model {
public:
    /// @brief Construct from AC3D Model
    ///
    /// Constructs a model from an AC3D File. Building edges is an
    /// optional operation. These are used by the shadowing renderer
    /// when you want to cast shadows.
    /// @param file - [in] The file object
    /// @param include_edges - [in] Set true to build edges
    Visible_model(const Utility::Ac3d_file& file, bool include_edges);

    Visible_model(const Visible_model&) = delete;
    Visible_model& operator=(const Visible_model&) = delete;

    /// @brief Model Surface
    ///
    /// A Model Surface object contains 3 indices into the Point
    ///	array.
    struct surface {
        surface(int16_t a, int16_t b, int16_t c, const Math::Local_unit_vector& n)
            : p0(a), p1(b), p2(c), normal(n)
        {
        }
        int16_t                 p0, p1, p2;
        Math::Local_unit_vector normal;
    };

    /// @brief A Model Edge
    ///
    /// The Edge structure is a simple thing used to keep
    ///	track of points and surfaces.  Each variable is an index
    ///	into the m_Points and m_Surfaces members
    struct Edge {
        unsigned short p0, p1;
        unsigned short s0, s1;
    };

    /// @brief Builds the edges
    ///
    /// Builds the internal edges structure.  This is used by
    ///	the shadow rendering system to construct silhouettes.
    void build_edges();

    const Math::Local_point&                           offset() const { return m_offset; }
    const std::vector<Math::Local_point>&              points() const { return m_points; }
    const std::vector<surface>&                        surfaces() const { return m_surfaces; }
    const std::vector<Edge>&                           edges() const { return m_edges; }
    const Color&                                       color() const { return m_color; }
    const std::vector<std::unique_ptr<Visible_model>>& kids() const { return m_kids; }

private:
    friend class RendererTest::Visible_model_test;

    Visible_model();

    void construct(const Utility::Ac3d_model&                 ac3d_model,
                   const std::vector<Utility::Ac3d_material>& materials, bool include_edges);
    void compute_point_normals();
    void add_edge(unsigned short p0, unsigned short p1, unsigned short s0, unsigned short s1);

    Math::Local_point                           m_offset;
    std::vector<Math::Local_point>              m_points;
    std::vector<Math::Local_unit_vector>        m_point_normals;
    std::vector<surface>                        m_surfaces;
    std::vector<Edge>                           m_edges;
    Color                                       m_color;
    std::vector<std::unique_ptr<Visible_model>> m_kids;
};

}  // namespace Renderer
}  // namespace Dubious
#endif
