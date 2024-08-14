#pragma once

#include "rdbus/Data.hpp"

namespace rdbus::out
{

class Output
{
public:
    virtual void send( const std::list< rdbus::Data >& list ) = 0;
    virtual void send( const rdbus::Data& data ) = 0;

    virtual ~Output() = default;
};

} // namespace rdbus::out
