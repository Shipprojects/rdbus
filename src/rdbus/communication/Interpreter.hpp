#pragma once

#include "rdbus/Exception.hpp"

namespace rdbus::communication::interpreter
{

struct Exception : rdbus::Exception
{
    Exception( const std::string& what )
    : rdbus::Exception( "Interpreter exception - " + what )
    {
    }
};

} // namespace rdbus::communication::interpreter