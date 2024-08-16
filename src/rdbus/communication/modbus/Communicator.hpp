#pragma once

#include "Adapter.hpp"
#include "rdbus/Data.hpp"
#include "rdbus/communication/Communicator.hpp"
#include "rdbus/communication/OSWrapper.hpp"
#include "rdbus/config/Serial.hpp"

namespace rdbus::communication::modbus
{

// A high level communication implementation for Modbus
class Communicator : public communication::Communicator
{
public:
    Communicator( const config::Serial& settings, std::unique_ptr< OS > os );

    rdbus::Data request( const config::Slave& slave ) override;

private:
    Adapter adapter;
};

} // namespace rdbus::communication::modbus
