#include "MB/modbusRequest.hpp"
#include "MB/modbusUtils.hpp"
#include "communication/modbus/MessageBuilder.hpp"
#include "config/Slave.hpp"
#include <gtest/gtest.h>

using namespace config;


TEST( TestMessageBuilder, TestWithCRC )
{
    const auto request = MB::ModbusRequest( 1, MB::utils::MBFunctionCode::ReadAnalogOutputHoldingRegisters, 100, 3 );

    Slave settings;
    settings.address = 1;
    settings.name = "slave";
    settings.pollTimeMs = std::chrono::milliseconds( 100 );
    settings.CRC = true;

    const auto& rawed = communication::modbus::toRawRequest( request, settings );

    decltype( rawed ) need = { 0x01, 0x03, 0x00, 0x64, 0x00, 0x03, 0x44, 0x14 };

    EXPECT_EQ( rawed, need );
}

TEST( TestMessageBuilder, TestWithoutCRC )
{
    const auto request = MB::ModbusRequest( 3, MB::utils::MBFunctionCode::ReadAnalogOutputHoldingRegisters, 40000, 34 );

    Slave settings;
    settings.address = 3;
    settings.name = "slave";
    settings.pollTimeMs = std::chrono::milliseconds( 100 );
    settings.CRC = false;

    const auto& rawed = communication::modbus::toRawRequest( request, settings );

    decltype( rawed ) need = { 0x03, 0x03, 0x9c, 0x40, 0x00, 0x22 };

    EXPECT_EQ( rawed, need );
}
