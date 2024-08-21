#pragma once

#include "rdbus/Data.hpp"
#include "rdbus/config/modbus/Register.hpp"
#include <MB/modbusResponse.hpp>
#include <stdint.h>

namespace rdbus::communication::modbus::interpreter
{

namespace tools
{

// Modbus data interpretation algorithm
//
// Modbus RTU protocol is encoded as big-endian, however, it only applies on it's metadata fields.
// There is no endianness enforcement on it's actual data fields, leaving it up to the developer or
// user to decide and implement. Data interpretation is implemented in this module and described
// here with the help of function declarations put into correct calling order.
//
// Underlaying Modbus library is made for standard 16-bit Modbus, which is not fully compatible
// with the required 32-bit Modbus. Additionally the Modbus library performs data interpretation
// by reading 16-bit words and swapping their bytes to convert assumed big-endian format to
// little-endian format so the program could use it. The approach is flawed, because the data stored
// in registers can be stored in various different ways, especially for 32-bit Modbus. So the
// correct approach is to interpret data using some external help or descriptions that are passed
// to the program.
// The first step in interpreting data is to swap back the order of 16-bit words, and convert
// it to an array for further processing.
using RawUint16List = std::vector< std::array< uint8_t, sizeof( uint16_t ) > >;
RawUint16List toRaw16BitRegisters( const std::vector< MB::ModbusCell >& );

using RawedBytesList = std::vector< std::vector< uint8_t > >;
using Registers = std::list< config::modbus::Register >;
// The second step is to merge some 16-bit registers into user defined groups according to the
// passed description of registers.
using RawMergedList = RawedBytesList;
RawMergedList toRawMergedRegisters( const RawUint16List&, const Registers& );
// The third step is to perform user interpretation of data. The user describes each register byte
// order in big-endian form. We get list of big-endian data.
using BigEndianRegisters = RawedBytesList;
BigEndianRegisters toUserInterpretation( const RawMergedList&, const Registers& );
// The fourth step is to convert the big-endian data to little-endian form for the machine to use.
using SmallEndianRegisters = RawedBytesList;
SmallEndianRegisters toMachineInterpretation( const SmallEndianRegisters& );
// The last step is to parse little-endian data into output fields.
using Fields = std::list< rdbus::Data::Field >;
using Timestamp = rdbus::Data::Field::Timestamp;
Fields toParsedFields( const SmallEndianRegisters&, const Registers&, const Timestamp& );

} // namespace tools

std::list< rdbus::Data::Field > parse( const MB::ModbusResponse&,
                                       const tools::Registers&,
                                       const tools::Timestamp& );

} // namespace rdbus::communication::modbus::interpreter
