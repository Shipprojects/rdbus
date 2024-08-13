#pragma once

#include "rdbus/config/Slave.hpp"
#include "rdbus/Data.hpp"

namespace rdbus::communication
{

class Communicator
{
public:
    virtual std::list< rdbus::Data > request( const rdbus::config::Slave& slave ) = 0;

    virtual ~Communicator() = default;
};

} // namespace communication
