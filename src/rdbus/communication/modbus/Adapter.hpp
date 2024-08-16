#pragma once

#include "rdbus/communication/Connection.hpp"
#include "rdbus/config/Serial.hpp"
#include <MB/modbusRequest.hpp>
#include <MB/modbusResponse.hpp>

namespace rdbus::communication::modbus
{

// A middleman that performs additional setup for each request/response
// before sending/receiving data
class Adapter
{
public:
    Adapter( const config::Serial& settings );

    using Request = MB::ModbusRequest;
    using Response = MB::ModbusResponse;
    using seconds = std::chrono::seconds;
    Response send( const Request&, seconds requestTimeout = seconds( 10 ) );

private:
    Connection< OSWrapper > connection;
};

} // namespace rdbus::communication::modbus
