#pragma once

#include "rdbus/Exception.hpp"
#include <string>

namespace rdbus::config
{

struct ParseException : public rdbus::Exception
{
    explicit ParseException( const std::string& what )
    : rdbus::Exception( "Config parse exception - " + what )
    {
    }
};

} // namespace rdbus::config
