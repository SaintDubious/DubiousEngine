#ifndef INCLUDED_RENDERER_MODEL
#define INCLUDED_RENDERER_MODEL

#include "Color.h"
#include "Point.h"
#include "UnitVector.h"

#include <vector>
#include <memory>

//////////////////////////////////////////////////////////////
namespace Dubious {

namespace Utility { 
class AC3DFile;
class AC3DModel; 
class AC3DMaterial;
typedef std::vector<AC3DMaterial> AC3DMaterialVector;
}

namespace Math {
struct Triple;
}

namespace Renderer {

class Model;
typedef std::shared_ptr<Model> ModelPtr;
typedef std::vector<ModelPtr> ModelPtrVector;
typedef std::vector<Math::LocalPoint> LocalPointVector;
typedef std::vector<Math::LocalUnitVector> LocalUnitVectorVector;

/// @brief Representation of a visible model
///
/// The Renderer::Model is a model suitable for drawing in
///	OpenGL.  It is similar to the AC3DModel, but it contains
///	more information, like point normals.
class Model
{
public:
    Model( const Model& ) = delete;

    /// @brief Construct from AC3D Model
    ///
    /// Constructs a model from an AC3D File. Building edges is an 
    /// optional operation. These are used by the shadowing renderer
    /// when you want to cast shadows.
    /// @param File - [in] The file object
    /// @param IncludeEdges - [in] Set true to build edges
    Model( const Utility::AC3DFile& File, bool IncludeEdges );

    /// @brief Construct a cube
    ///
    /// Builds a cube of the given dimensions with the origin
    /// centered. If IncludeEdges is true then it will build
    /// edge information, used in shadow rendering
    /// @param File - [in] The file object
    /// @param IncludeEdges - [in] Set true to build edges
    Model( const Math::Triple& Dimensions, bool IncludeEdges );

    /// @brief Default destructor
    ~Model() = default;

    Model& operator=( const Model& ) = delete;

    /// @brief Model Surface
    ///
    /// A Model Surface object contains 3 indices into the Point
    ///	array.  
    struct Surface
    {
        Surface( int16_t a, int16_t b, int16_t c, const Math::LocalUnitVector& n )
            : p0( a )
            , p1( b )
            , p2( c )
            , Normal( n )
        {}
        int16_t  p0, p1, p2;
        Math::LocalUnitVector Normal;
    };
    typedef std::vector<Surface> SurfaceVector;

    /// @brief A Model Edge
    ///
    /// The Edge structure is a simple thing used to keep
    ///	track of points and surfaces.  Each variable is an index
    ///	into the m_Points and m_Surfaces members
    struct Edge
    {
        unsigned short  p0, p1;
        unsigned short  s0, s1;
    };
    typedef std::vector<Edge> EdgeVector;

    /// @brief Builds the edges
    ///
    /// Builds the internal edges structure.  This is used by
    ///	the shadow rendering system to construct silhouettes.
    void				BuildEdges();

    const Math::LocalPoint& Offset() const { return m_Offset; }
    const LocalPointVector& Points() const { return m_Points; }
    const SurfaceVector& Surfaces() const { return m_Surfaces; }
    const EdgeVector&	Edges() const { return m_Edges; }
    const Color&		MaterialColor() const { return m_Color; }
    const ModelPtrVector& Kids() const { return m_Kids; }
private:

    Model();

    void				Construct( const Utility::AC3DModel& AC3DModel, const Utility::AC3DMaterialVector& Materials, bool IncludeEdges );
    void				ComputePointNormals();
    void				AddEdge( unsigned short p0, unsigned short p1, unsigned short s0, unsigned short s1 );

    Math::LocalPoint	m_Offset;
    LocalPointVector	m_Points;
    LocalUnitVectorVector m_PointNormals;
    SurfaceVector		m_Surfaces;
    EdgeVector			m_Edges;
    Color				m_Color;
    ModelPtrVector		m_Kids;

};

}
}
#endif
