#pragma once

#include "rdbus/communication/Connection.hpp"
#include "rdbus/communication/OSWrapper.hpp"
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
    Adapter( const config::Serial& settings, std::unique_ptr< OS > os );

    using Request = MB::ModbusRequest;
    using Response = MB::ModbusResponse;
    using seconds = std::chrono::seconds;
    Response send( const Request& );

private:
    Connection connection;
};

} // namespace rdbus::communication::modbus
