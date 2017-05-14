#ifndef INCLUDED_UTILITY_AC3DFILEREADER
#define INCLUDED_UTILITY_AC3DFILEREADER

#include <Point.h>

#include <string>
#include <memory>
#include <vector>

namespace Dubious {
namespace Utility {

/// @brief An AC3D Model
///
/// Respresents a model as described by the AC3D file format.
///	AC3D files contains a number of points and then a list of
///	surfaces that index those points.  For example if point number
///	10 is used in 3 surfaces, then all of those surfaces will include
///	the number 10 in their description.  To put the model back
///	together, you need the point array and the surface array.
///	That info is stored in this object.
class Ac3d_model {
public:
    Ac3d_model()                  = default;
    Ac3d_model(const Ac3d_model&) = delete;
    Ac3d_model& operator=(const Ac3d_model&) = delete;

    /// @brief A Surface
    ///
    /// An AC3D Surface object contains 3 indices into the Point
    ///	array.  Actually AC3D supports any number of points, but I
    ///	restrict the reader to only accepting a model made of triangles.
    ///	It also contains Material information.
    struct Surface {
        unsigned short p0, p1, p2;
        int            material_index;
    };

    /// @brief Accessor - the Name, mostly useful for debugging
    std::string& name() { return m_name; }

    /// @brief Accessor - Offset from the parent model.  If this is
    ///		the parent model then the offset will be (0,0,0)
    Math::Local_point&       offset() { return m_offset; }
    const Math::Local_point& offset() const { return m_offset; }

    /// @brief Accessor - the Points that make up the model.
    std::vector<Math::Local_point>&       points() { return m_points; }
    const std::vector<Math::Local_point>& points() const { return m_points; }

    /// @brief Accessor - the surfaces that make up the model
    std::vector<Surface>&       surfaces() { return m_surfaces; }
    const std::vector<Surface>& surfaces() const { return m_surfaces; }

    /// @brief Accessor - child models
    const std::vector<std::unique_ptr<const Ac3d_model>>& kids() const { return m_kids; }
    void push_kid(std::unique_ptr<const Ac3d_model> kid) { m_kids.push_back(std::move(kid)); }

private:
    std::string                                    m_name;
    Math::Local_point                              m_offset;
    std::vector<Math::Local_point>                 m_points;
    std::vector<Surface>                           m_surfaces;
    std::vector<std::unique_ptr<const Ac3d_model>> m_kids;
};

/// @brief A Material in an AC3D file
///
/// The AC3DMaterial object actually contains a lot of information
///	about color and lighting.  However at this point I'm only interested
///	in the base color, so that's all I store.
class Ac3d_material {
public:
    /// @brief A simple 3 channel color.
    ///
    /// Each channel is described by a number between 0.0 and 1.0
    struct Color {
        /// @brief Constructor - creates the specified RGB color
        Color(float r, float g, float b) : red(r), green(g), blue(b) {}
        float red, green, blue;
    };

    /// @brief Constructor - creates a material from the specified color
    /// @param MaterialColor - [in] the color of the material
    Ac3d_material(const Color& color) : m_color(color) {}

    /// @brief Accessor - the material color
    const Color& color() const { return m_color; }

private:
    Color m_color;
};

/// @brief Abstraction of the AC3D File
///
/// The AC3DFile class is meant to represent all of the information
///	stored in an AC3D file.  This means the material information as well
///	as the model.
class Ac3d_file {
public:
    /// @brief Constructor - saves the specified Model and Materials
    /// @param Materials - [in] Materials vector
    /// @param pModel - [in] the model
    Ac3d_file(std::vector<Ac3d_material>&& materials, std::unique_ptr<const Ac3d_model> model)
        : m_model(std::move(model)), m_materials(materials)
    {
    }

    Ac3d_file(const Ac3d_file&) = delete;
    Ac3d_file& operator=(const Ac3d_file&) = delete;

    /// @brief Accessor - the Model itself
    std::shared_ptr<const Ac3d_model> model() const { return m_model; }

    /// @brief Accessor - the materials used in the model
    const std::vector<Ac3d_material>& materials() const { return m_materials; }

private:
    std::shared_ptr<const Ac3d_model> m_model;
    std::vector<Ac3d_material>        m_materials;
};

class File_path;

/// @brief An object that can read an AC3D file
///
/// This class can be used to read AC3D files into an AC3DFile
///	object.  Note that it is not a robust solution.  It does not support
///	all features, and it only supports specifically created models.  One
///	important limitation is that a model must be composed only of triangles.
///	Also, you must choose the Re-Center option for your model so that all
///	children will be centered around their middle.
///	This class can not be created, it can only be used to read a file.
class Ac3d_file_reader {
public:
    Ac3d_file_reader()                        = delete;
    ~Ac3d_file_reader()                       = delete;
    Ac3d_file_reader(const Ac3d_file_reader&) = delete;
    Ac3d_file_reader& operator=(const Ac3d_file_reader&) = delete;

    /// @brief Read the specified file and return an AC3DFilePtr
    /// @param FileName - [in] path and file name of the AC3D file
    /// @returns a pointer to the file information
    static std::unique_ptr<const Ac3d_file> read_file(const File_path& file_name);

    /// @brief Create a test cube
    /// @param edge - [in] edge length
    /// @returns a pointer to the file information
    static std::unique_ptr<const Ac3d_file> test_cube(float width, float height, float depth);

    /// @brief Create a test model that has 3 cubes in a group
    /// @param edge - [in] edge length
    /// @returns a pointer to the file information
    static std::unique_ptr<const Ac3d_file> test_cube_group(float edge_length);
};

}  // namespace Utility
}  // namespace Dubious

#endif
