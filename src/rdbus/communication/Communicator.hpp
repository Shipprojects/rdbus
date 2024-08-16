#pragma once

#include "rdbus/Data.hpp"
#include "rdbus/config/Slave.hpp"

namespace rdbus::communication
{

// Placeholder for communication implementations for different protocols
class Communicator
{
public:
    virtual rdbus::Data request( const rdbus::config::Slave& slave ) = 0;

    virtual ~Communicator() = default;
};

} // namespace rdbus::communication
