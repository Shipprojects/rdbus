#pragma once

#include "Adapter.hpp"
#include "rdbus/communication/Communicator.hpp"
#include "rdbus/config/Serial.hpp"

namespace communication::modbus
{

class Communicator : public communication::Communicator
{
public:
    Communicator( const config::Serial& settings );

    void request( const config::Slave& slave ) override;

private:
    Adapter adapter;
};

} // namespace communication::modbus
