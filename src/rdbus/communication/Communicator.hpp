#pragma once

#include "rdbus/config/Slave.hpp"

namespace communication
{

class Communicator
{
public:
    virtual void request( const config::Slave& slave ) = 0;

    virtual ~Communicator() = default;
};

} // namespace communication
