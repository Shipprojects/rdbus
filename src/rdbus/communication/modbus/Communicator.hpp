#pragma once

#include "Adapter.hpp"
#include "rdbus/Data.hpp"
#include "rdbus/communication/Communicator.hpp"
#include "rdbus/config/Serial.hpp"

namespace communication::modbus
{

class Communicator : public communication::Communicator
{
public:
    Communicator( const config::Serial& settings );

    std::list< rdbus::Output > request( const config::Slave& slave ) override;

private:
    Adapter adapter;
};

} // namespace communication::modbus
