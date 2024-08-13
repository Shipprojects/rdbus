#pragma once

#include "rdbus/config/Register.hpp"
#include <MB/modbusRequest.hpp>
#include <list>

namespace rdbus::communication::modbus
{

struct RequestDescription
{
    MB::ModbusRequest request;
    std::list< config::Register > registers;
};

using RequestDescriptions = std::list< RequestDescription >;

} // namespace rdbus::communication::modbus
