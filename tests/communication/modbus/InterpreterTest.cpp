#include "rdbus/communication/modbus/Interpreter.hpp"
#include "rdbus/config/modbus/Register.hpp"
#include <MB/modbusResponse.hpp>
#include <chrono>
#include <gtest/gtest.h>

using namespace rdbus;
using namespace rdbus::config::modbus;
using namespace communication::modbus::interpreter;

TEST( TestInterpreter, TestToRaw16BitRegisters )
{
    const std::vector< uint8_t > input = { 0x02, 0x03, 0x0c, 0x0e, 0x0a, 0x5e, 0xcd, 0xe9, 0xad, 0x0d, 0x9f, 0xa3, 0x94, 0x1a, 0x00 };
    const auto& response = MB::ModbusResponse::fromRaw( input );

    const auto& raw16BitRegisters = tools::toRaw16BitRegisters( response.registerValues() );

    const tools::RawUint16List need = { { 0x0e, 0x0a }, { 0x5e, 0xcd }, { 0xe9, 0xad }, { 0x0d, 0x9f }, { 0xa3, 0x94 }, { 0x1a, 0x00 } };
    EXPECT_EQ( raw16BitRegisters, need );
}

TEST( TestInterpreter, TestToRawMergedRegisters )
{
    std::list< Register > registers;
    {
        Register reg;
        reg.address = 100;
        reg.byteOrder = { 2, 3, 0, 1, 6, 7, 4, 5 };
        reg.type = Type::Double;
        reg.name = "Register_A";
        registers.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 104;
        reg.byteOrder = { 1, 0 };
        reg.type = Type::Uint16;
        reg.name = "Register_B";

        registers.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 105;
        reg.byteOrder = { 2, 3, 0, 1 };
        reg.type = Type::Uint32;
        reg.name = "Register_C";

        registers.emplace_back( std::move( reg ) );
    }

    const tools::RawUint16List raw16BitRegisters = { { 0x8f, 0xb9 }, { 0x05, 0x88 }, { 0x3f, 0xca }, { 0xdb, 0xe5 }, { 0x5d, 0x68 }, { 0x82, 0x74 }, { 0x0d, 0xc2 } };
    const auto& rawMergedRegisters = tools::toRawMergedRegisters( raw16BitRegisters, registers );

    const tools::RawMergedList need = { { 0x8f, 0xb9, 0x05, 0x88, 0x3f, 0xca, 0xdb, 0xe5 }, { 0x5d, 0x68 }, { 0x82, 0x74, 0x0d, 0xc2 } };
    EXPECT_EQ( rawMergedRegisters.size(), 3 );
    EXPECT_EQ( rawMergedRegisters, need );
}

TEST( TestInterpreter, TestToUserInterpretation )
{
    std::list< Register > registers;
    {
        Register reg;
        reg.address = 100;
        reg.byteOrder = { 2, 3, 0, 1, 6, 7, 4, 5 };
        reg.type = Type::Double;
        reg.name = "Register_A";
        registers.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 104;
        reg.byteOrder = { 1, 0 };
        reg.type = Type::Uint16;
        reg.name = "Register_B";

        registers.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 105;
        reg.byteOrder = { 2, 3, 0, 1 };
        reg.type = Type::Uint32;
        reg.name = "Register_C";

        registers.emplace_back( std::move( reg ) );
    }

    const tools::RawMergedList rawMergedRegisters = { { 0x8f, 0xb9, 0x05, 0x88, 0x3f, 0xca, 0xdb, 0xe5 },
                                                      { 0x5d, 0x68 },
                                                      { 0x82, 0x74, 0x0d, 0xc2 } };
    const auto& bigEndianRegisters = tools::toUserInterpretation( rawMergedRegisters, registers );

    const tools::BigEndianRegisters need = { { 0x05, 0x88, 0x8f, 0xb9, 0xdb, 0xe5, 0x3f, 0xca },
                                             { 0x68, 0x5d },
                                             { 0x0d, 0xc2, 0x82, 0x74 } };
    EXPECT_EQ( bigEndianRegisters, need );
}

TEST( TestInterpreter, TestToMachineInterpretation )
{
    const tools::BigEndianRegisters bigEndianRegisters = { { 0x05, 0x88, 0x8f, 0xb9, 0xdb, 0xe5, 0x3f, 0xca }, { 0x68, 0x5d }, { 0x0d, 0xc2, 0x82, 0x74 } };
    const auto& littleEndianRegisters = tools::toMachineInterpretation( bigEndianRegisters );

    const tools::SmallEndianRegisters need = { { 0xca, 0x3f, 0xe5, 0xdb, 0xb9, 0x8f, 0x88, 0x05 }, { 0x5d, 0x68 }, { 0x74, 0x82, 0xc2, 0x0d } };
    EXPECT_EQ( littleEndianRegisters, need );
}

TEST( TestInterpreter, TestToParsedFields )
{
    const std::vector< uint8_t > input = { 0x02, 0x03, 0x12, 0x8f, 0xb9, 0x05, 0x88, 0x3f, 0xca, 0xdb, 0xe5, 0x82, 0x74, 0x0d, 0xc2, 0x5d, 0x68, 0xe5, 0xff, 0x54, 0xbf };
    const auto& response = MB::ModbusResponse::fromRaw( input );

    std::list< Register > registers;
    {
        Register reg;
        reg.address = 100;
        reg.byteOrder = { 2, 3, 0, 1 };
        reg.type = Type::Uint32;
        reg.name = "Register_A";

        registers.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 102;
        reg.byteOrder = { 0, 1, 2, 3, 4, 5, 6, 7 };
        reg.type = Type::Double;
        reg.name = "Register_B";
        registers.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 106;
        reg.byteOrder = { 1, 0 };
        reg.type = Type::Uint16;
        reg.name = "Register_C";
        registers.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 107;
        reg.byteOrder = { 1, 0, 3, 2 };
        reg.type = Type::Int32;
        reg.name = "Register_D";
        registers.emplace_back( std::move( reg ) );
    }

    const auto& now = std::chrono::system_clock::now();
    const auto& fields = communication::modbus::interpreter::parse( response, registers, now );

    ASSERT_EQ( fields.size(), 4 );

    auto it = fields.begin();
    EXPECT_EQ( it->name, "Register_A" );
    ASSERT_TRUE( it->value.has_value() );
    ASSERT_TRUE( std::holds_alternative< uint32_t >( it->value.value() ) );
    EXPECT_EQ( std::get< uint32_t >( *it->value ), 92835769 );
    EXPECT_EQ( it->timestamp, now );

    it++;
    EXPECT_EQ( it->name, "Register_B" );
    ASSERT_TRUE( it->value.has_value() );
    ASSERT_TRUE( std::holds_alternative< double >( it->value.value() ) );
    EXPECT_EQ( std::get< double >( *it->value ), 0.20983570928375 );
    EXPECT_EQ( it->timestamp, now );

    it++;
    EXPECT_EQ( it->name, "Register_C" );
    ASSERT_TRUE( it->value.has_value() );
    ASSERT_TRUE( std::holds_alternative< uint16_t >( it->value.value() ) );
    EXPECT_EQ( std::get< uint16_t >( *it->value ), 26717 );
    EXPECT_EQ( it->timestamp, now );

    it++;
    EXPECT_EQ( it->name, "Register_D" );
    ASSERT_TRUE( it->value.has_value() );
    ASSERT_TRUE( std::holds_alternative< int32_t >( it->value.value() ) );
    EXPECT_EQ( std::get< int32_t >( *it->value ), -1720492 );
    EXPECT_EQ( it->timestamp, now );
}
