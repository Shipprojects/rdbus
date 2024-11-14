#pragma once

#include "rdbus/Exception.hpp"

namespace rdbus::out
{

struct Exception : public rdbus::Exception
{
    Exception( const std::string& what )
    : rdbus::Exception( "Output exception - " + what )
    {
    }
};

} // namespace rdbus::out
