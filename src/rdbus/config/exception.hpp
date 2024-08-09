#pragma once

#include <stdexcept>
#include <string>

namespace config
{

struct ParseException : public std::runtime_error
{
    ParseException( const std::string& what )
    : std::runtime_error( what )
    {
    }
};

} // namespace config
