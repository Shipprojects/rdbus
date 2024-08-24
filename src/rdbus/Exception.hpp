#pragma once

#include <stdexcept>

namespace rdbus
{

// This is main rdbus exception. All rdbus related exceptions inherit this exception.
struct Exception : public std::runtime_error
{
    Exception( const std::string& what )
    : std::runtime_error( what )
    {
    }
};

} // namespace rdbus
