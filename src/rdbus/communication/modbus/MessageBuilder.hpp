#pragma once

#include "config/Slave.hpp"
#include <MB/modbusRequest.hpp>
#include <vector>

namespace communication
{

namespace modbus
{

// Converts request object to raw data, adjusting it to settings
std::vector< uint8_t > toRawRequest( const MB::ModbusRequest& request, const config::Slave& settings );

} // namespace modbus

} // namespace communication