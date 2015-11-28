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
class FilePath
{
public:
    /// @brief Constructor.  
    ///
    /// Takes in a Path in the usual form.  It's 
    ///	smart enough to figure out if it's a full or relative path.
    ///	Passing it an empty string will throw an exception
    /// @param Path - [in] the path: /blah/blah/file.txt
    FilePath( const std::string& Path );

    /// @brief Default copy constructor
    FilePath( const FilePath& ) = default;

    /// @brief Destructor
    ~FilePath() = default;

    /// @brief Default assignment operator
    FilePath& operator=( const FilePath& ) = default;

    /// @brief Returns the full path that you specified to the constructor
    /// @returns the full path: /blah/blah/file.txt
    std::string			FullPath() const;

    /// @brief Returns just the path part of the full path.  
    ///
    /// This means everything except the final token.  Note that this will end
    ///	in a final '/'
    /// @returns the path only: /blah/blah/
    std::string			Path() const;

    /// @brief Returns just the file part.  This means only the last token
    /// @returns the file only: file.txt
    std::string			File() const;

private:
    std::vector<std::string> m_Parts;
    bool				m_Absolute;
};

std::ostream& operator<<( std::ostream& os, const FilePath& Input );

}
}

#endif
