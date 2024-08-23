#pragma once

#include <stdexcept>
#include <string>

namespace rdbus::config
{

struct ParseException : public std::runtime_error
{
    explicit ParseException( const std::string& what )
    : std::runtime_error( "Config parse exception - " + what )
    {
    }
};

} // namespace rdbus::config
