#ifndef INCLUDED_UTILITY_FILEPATH
#define INCLUDED_UTILITY_FILEPATH

#include <string>
#include <vector>

//////////////////////////////////////////////////////////////
namespace Dubious {
namespace Utility {

/// @brief Path and Filename information.
///
/// The FilePath object is used to hold path and file information.  
///	This is a simple helper class that just moves all of the annoying
///	path parsing into one central location.  It only uses the '/' 
///	character as a separator.
class File_path {
public:
    /// @brief Constructor.  
    ///
    /// Takes in a Path in the usual form.  It's 
    ///	smart enough to figure out if it's a full or relative path.
    ///	Passing it an empty string will throw an exception
    /// @param Path - [in] the path: /blah/blah/file.txt
    File_path( const std::string& Path );

    /// @brief Returns the full path that you specified to the constructor
    /// @returns the full path: /blah/blah/file.txt
    std::string			full_path() const;

    /// @brief Returns just the path part of the full path.  
    ///
    /// This means everything except the final token.  Note that this will end
    ///	in a final '/'
    /// @returns the path only: /blah/blah/
    std::string			path() const;

    /// @brief Returns just the file part.  This means only the last token
    /// @returns the file only: file.txt
    std::string			file() const;

private:
    std::vector<std::string> m_parts;
    bool				m_absolute;
};

std::ostream& operator<<( std::ostream& os, const File_path& p );

}
}

#endif
