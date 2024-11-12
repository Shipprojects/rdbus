#pragma once

#include "rdbus/Data.hpp"
#include "rdbus/config/Address.hpp"
#include "rdbus/config/eip/Module.hpp"

namespace rdbus::communication::wago
{

// A high level communication implementation for Wago PLCs
class Communicator
{
public:
    Communicator( const config::Address& address );

    rdbus::Data request( const config::eip::Module& module );

private:
    const config::Address address;
};

} // namespace rdbus::communication::wago
