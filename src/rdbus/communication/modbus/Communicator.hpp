#pragma once

#include "Adapter.hpp"
#include "rdbus/Data.hpp"
#include "rdbus/communication/OSWrapper.hpp"
#include "rdbus/config/Serial.hpp"
#include "rdbus/config/modbus/Slave.hpp"

namespace rdbus::communication::modbus
{

// A high level communication implementation for Modbus
class Communicator
{
public:
    Communicator( const config::Serial& settings, std::unique_ptr< OS > os );

    rdbus::Data request( const config::modbus::Slave& slave );

private:
    Adapter adapter;
};

} // namespace rdbus::communication::modbus
