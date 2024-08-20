#pragma once

#include "rdbus/config/Slave.hpp"
#include <MB/modbusRequest.hpp>

namespace rdbus::communication::modbus
{

struct RequestDescription
{
    MB::ModbusRequest request;
    std::list< config::Register > registers;
};

using RequestDescriptions = std::vector< RequestDescription >;

// Returns a list of modbus requests to be executed sequentually. A single requests will
// be split into many if there are different kind of registers that have to be read and/or
// due to the possibility in configuration to specify registers that are not directly
// adjacent to each other. This limitation comes from Modbus protocol itself, where
// reading happens from x address for n registers.
RequestDescriptions requestPlan( const config::Slave& slave );

} // namespace rdbus::communication::modbus