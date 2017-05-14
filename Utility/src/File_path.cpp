#include "File_path.h"
#include <sstream>

namespace Dubious {
namespace Utility {

namespace {

std::vector<std::string>
tokenizer(const std::string& input)
{
    std::vector<std::string> result;
    int                      start_pos = 0;
    if (input[0] == '/') {
        start_pos = 1;
    }
    int end_pos = input.find("/", start_pos);
    while (true) {
        if (end_pos == std::string::npos) {
            result.push_back(input.substr(start_pos));
            break;
        }
        else {
            result.push_back(input.substr(start_pos, end_pos - start_pos));
        }
        start_pos = end_pos + 1;
        end_pos   = input.find("/", start_pos);
    }
    return result;
}

}  // namespace

File_path::File_path(const std::string& path)
{
    if (path.length() == 0) {
        throw std::runtime_error("Passed an empty path to the File_path object");
    }
    m_parts    = tokenizer(path);
    m_absolute = (path[0] == '/');
}

std::string
File_path::full_path() const
{
    return path() + file();
}

std::string
File_path::path() const
{
    std::ostringstream result;
    if (m_absolute) {
        result << "/";
    }
    for (size_t i = 0; i < m_parts.size() - 1; ++i) {
        result << m_parts[i] << "/";
    }
    return result.str();
}

std::string
File_path::file() const
{
    return m_parts.back();
}

std::ostream&
operator<<(std::ostream& os, const File_path& Input)
{
    os << Input.full_path();
    return os;
}

}  // namespace Utility
}  // namespace Dubious
