#pragma once

#include "rdbus/Data.hpp"
#include "rdbus/config/Register.hpp"
#include <MB/modbusResponse.hpp>
#include <limits>
#include <stdint.h>

namespace communication::modbus::interpreter
{

// Not meant for usage outside of Interpreter!
namespace tools
{

// Modbus data interpretation algorithm
// TODO: describe the algorithm

using RawUint16List = std::vector< std::array< uint8_t, sizeof( uint16_t ) > >;
RawUint16List toRaw16BitRegisters( const std::vector< MB::ModbusCell >& );

using RawUint32List = std::vector< std::array< uint8_t, sizeof( uint32_t ) > >;
RawUint32List toRaw32BitRegisters( const RawUint16List& );

using RawedBytesList = std::vector< std::vector< uint8_t > >;
using Registers = std::list< config::Register >;

using RawMergedList = RawedBytesList;
RawMergedList toRawMergedRegisters( const RawUint32List&, const Registers& );

using BigEndianRegisters = RawedBytesList;
BigEndianRegisters toUserInterpretation( const RawMergedList&, const Registers& );

using SmallEndianRegisters = RawedBytesList;
SmallEndianRegisters toMachineInterpretation( const SmallEndianRegisters& );

using Fields = std::list< rdbus::Output::Field >;
using Timestamp = rdbus::Output::Field::Timestamp;
Fields toParsedFields( const SmallEndianRegisters&, const Registers&, const Timestamp& );

} // namespace tools

std::list< rdbus::Output::Field > parse( const MB::ModbusResponse&,
                                         const tools::Registers&,
                                         const tools::Timestamp& );

} // namespace communication::modbus::interpreter
