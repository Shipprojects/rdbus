#pragma once

#include "rdbus/config/Slave.hpp"
#include "rdbus/Data.hpp"

namespace communication
{

class Communicator
{
public:
    virtual std::list< rdbus::Output > request( const config::Slave& slave ) = 0;

    virtual ~Communicator() = default;
};

} // namespace communication
