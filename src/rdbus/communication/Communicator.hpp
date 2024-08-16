#pragma once

#include "rdbus/Data.hpp"
#include "rdbus/config/Slave.hpp"

namespace rdbus::communication
{

// High level interface between user and device.
class Communicator
{
public:
    virtual rdbus::Data request( const rdbus::config::Slave& slave ) = 0;

    virtual ~Communicator() = default;
};

} // namespace rdbus::communication
