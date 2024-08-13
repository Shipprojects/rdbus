#pragma once

#include "RequestDescription.hpp"
#include "rdbus/config/Slave.hpp"
#include <MB/modbusRequest.hpp>

namespace communication::modbus
{

// Returns a list of modbus requests to be executed sequentually.
// Due to the possibility in configuration to specify registers that are not directly
// adjacent to each other, we have to execute multiple read requests to read all
// necessary values. This limitation comes from Modbus protocol itself, where
// reading happens from x address for n registers.
RequestDescriptions requestPlan( const config::Slave& slave );

} // namespace communication::modbus