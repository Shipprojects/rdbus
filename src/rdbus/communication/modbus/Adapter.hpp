#pragma once

#include "communication/Connection.hpp"
#include "config/Serial.hpp"
#include <MB/modbusRequest.hpp>
#include <MB/modbusResponse.hpp>

namespace communication::modbus
{

class Adapter
{
public:
    Adapter( const config::Serial& settings );

    using Request = MB::ModbusRequest;
    using Response = MB::ModbusResponse;
    Response request( const Request& );

private:
    Connection< OSWrapper > connection;
};

} // namespace communication::modbus
