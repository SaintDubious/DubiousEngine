#ifndef INCLUDED_UTILITY_AC3DFILEREADER
#define INCLUDED_UTILITY_AC3DFILEREADER

#include <string>
#include <memory>
#include <vector>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Utility {

class AC3DModel;
typedef std::shared_ptr<AC3DModel> AC3DModelPtr;
typedef std::vector<AC3DModelPtr> AC3DModelPtrVector;

/// @brief An AC3D Model
/// 
/// Respresents a model as described by the AC3D file format.
///	AC3D files contains a number of points and then a list of
///	surfaces that index those points.  For example if point number
///	10 is used in 3 surfaces, then all of those surfaces will include
///	the number 10 in their description.  To put the model back
///	together, you need the point array and the surface array.
///	That info is stored in this object.
class AC3DModel
{
public:
	/// @brief Constructor.  
	///
	/// Creates a model at (0,0,0) that contains
	///	no points, surfaces, or children
	AC3DModel() {}

	/// @brief Destructor.
	~AC3DModel() {}

	/// @brief Simple 3 float point.  
	///
	/// This is used to represent a 3D position in a AC3D file.
	struct Point
	{
		/// @brief Constructor creates a point at (0,0,0)
		Point() : X(0), Y(0), Z(0) 
		{}

		/// @brief Constructor creates a point at specified (x,y,z)
		Point( float x, float y, float z )
			: X( x ), Y( y ), Z( z ) 
		{}

		float			X, Y, Z;
	};
	typedef std::vector<Point> PointVector;

	/// @brief A Surface
	///
	/// An AC3D Surface object contains 3 indices into the Point
	///	array.  Actually AC3D supports any number of points, but I
	///	restrict the reader to only accepting a model made of triangles.
	///	It also contains Material information.
	struct Surface
	{
		unsigned short  p0, p1, p2;
		int             MaterialIndex;
	};
	typedef std::vector<Surface> SurfaceVector;

	/// @brief Accessor - the Name, mostly useful for debugging
	std::string&		Name() { return m_Name; }

	/// @brief Accessor - Offset from the parent model.  If this is
	///		the parent model then the offset will be (0,0,0)
	Point&				Offset() { return m_Offset; }
	const Point&		Offset() const { return m_Offset; }

	/// @brief Accessor - the Points that make up the model.
	PointVector&		Points() { return m_Points; }
	const PointVector&	Points() const { return m_Points; }

	/// @brief Accessor - the surfaces that make up the model
	SurfaceVector&		Surfaces() { return m_Surfaces; }
	const SurfaceVector& Surfaces() const { return m_Surfaces; }

	/// @brief Accessor - child models
	AC3DModelPtrVector&	Kids() { return m_Kids; }
	const AC3DModelPtrVector& Kids() const { return m_Kids; }

private:
	AC3DModel( const AC3DModel& );
	AC3DModel& operator=( const AC3DModel& );

	std::string			m_Name;
	Point				m_Offset;
	PointVector			m_Points;
	SurfaceVector		m_Surfaces;
	AC3DModelPtrVector  m_Kids;
};

/// @brief A Material in an AC3D file
///
/// The AC3DMaterial object actually contains a lot of information
///	about color and lighting.  However at this point I'm only interested
///	in the base color, so that's all I store.
class AC3DMaterial
{
public:
	/// @brief A simple 3 channel color.  
	///
	/// Each channel is described by a number between 0.0 and 1.0
	struct Color 
	{
		/// @brief Constructor - creates the specified RGB color
		Color( float R, float G, float B )
			: Red( R ), Green( G ), Blue( B )
		{
		}

		float Red, Green, Blue;
	};

	/// @brief Constructor - creates a material from the specified color
	/// @param MaterialColor - [in] the color of the material
	AC3DMaterial( Color MaterialColor )
		: m_MaterialColor( MaterialColor )
	{
	}

	/// @brief Accessor - the material color
	const Color&		MaterialColor() const { return m_MaterialColor; }

private:
	Color				m_MaterialColor;
};

typedef std::vector<AC3DMaterial> AC3DMaterialVector;

/// @brief Abstraction of the AC3D File
/// 
/// The AC3DFile class is meant to represent all of the information
///	stored in an AC3D file.  This means the material information as well
///	as the model.
class AC3DFile
{
public:
	/// @brief Constructor - saves the specified Model and Materials
	/// @param Materials - [in] Materials vector
	/// @param pModel - [in] the model
	AC3DFile( const AC3DMaterialVector& Materials, AC3DModelPtr pModel )
		: m_pModel( pModel )
		, m_Materials( Materials )
	{}

	/// @brief Destructor
	~AC3DFile() {}

	/// @brief Accessor - the Model itself
	AC3DModelPtr		Model() const { return m_pModel; }

	/// @brief Accessor - the materials used in the model
	const AC3DMaterialVector& Materials() const { return m_Materials; }

private:
	AC3DFile( const AC3DFile& );
	AC3DFile& operator=( const AC3DFile& );

	AC3DModelPtr		m_pModel;
	AC3DMaterialVector	m_Materials;
};

typedef std::shared_ptr<AC3DFile> AC3DFilePtr;
class FilePath;

/// @brief An object that can read an AC3D file
///
/// This class can be used to read AC3D files into an AC3DFile
///	object.  Note that it is not a robust solution.  It does not support
///	all features, and it only supports specifically created models.  One
///	important limitation is that a model must be composed only of triangles.
///	Also, you must choose the Re-Center option for your model so that all
///	children will be centered around their middle.
///	This class can not be created, it can only be used to read a file.
class AC3DFileReader
{
public:
	/// @brief Read the specified file and return an AC3DFilePtr
	/// @param FileName - [in] path and file name of the AC3D file
	/// @returns a pointer to the file information
	static AC3DFilePtr ReadFile( const FilePath& FileName );

private:
	AC3DFileReader();
	~AC3DFileReader();
	AC3DFileReader( const AC3DFileReader& );
	AC3DFileReader& operator=( const AC3DFileReader& );

};

}
}

#endif
