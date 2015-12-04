#include "FilePath.h"
#include <sstream>

//////////////////////////////////////////////////////////////
using Dubious::Utility::FilePath;

namespace 
{

//////////////////////////////////////////////////////////////
std::vector<std::string> Tokenizer( const std::string& Input )
{
    std::vector<std::string> Result;
    int StartPos = 0;
    if (Input[0] == '/') {
        StartPos = 1;
    }
    int EndPos   = Input.find( "/", StartPos );
    while (true) {
        if (EndPos == std::string::npos) {
            Result.push_back( Input.substr( StartPos ) );
            break;
        }
        else {
            Result.push_back( Input.substr( StartPos, EndPos-StartPos ) );
        }
        StartPos = EndPos+1;
        EndPos = Input.find( "/", StartPos );
    }
    return Result;
}

}

//////////////////////////////////////////////////////////////
FilePath::FilePath( const std::string& Path )
{
    if (Path.length() == 0) {
        throw std::runtime_error( "Passed an empty path to the FilePath object" );
    }
    m_Parts		= Tokenizer( Path );
    m_Absolute	= (Path[0] == '/');
}

//////////////////////////////////////////////////////////////
std::string FilePath::FullPath() const
{
    return Path() + File();
}

//////////////////////////////////////////////////////////////
std::string FilePath::Path() const
{
    std::ostringstream Result;
    if (m_Absolute) {
        Result << "/";
    }
    for (size_t i=0; i<m_Parts.size()-1; ++i) {
        Result << m_Parts[i] << "/";
    }
    return Result.str();
}

//////////////////////////////////////////////////////////////
std::string FilePath::File() const
{
    return m_Parts[m_Parts.size()-1];
}

//////////////////////////////////////////////////////////////
std::ostream& operator<<( std::ostream& os, const FilePath& Input )
{
    os << Input.FullPath();
    return os;
}
