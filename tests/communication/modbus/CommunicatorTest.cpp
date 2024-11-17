#include "rdbus/communication/modbus/Communicator.hpp"
#include "communication/OSMock.hpp"
#include "rdbus/Data.hpp"
#include "rdbus/config/Serial.hpp"
#include "rdbus/config/modbus/Register.hpp"
#include "rdbus/config/modbus/Slave.hpp"
#include "gmock/gmock.h"
#include <chrono>
#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace rdbus::config::modbus;
using namespace rdbus::config;
using namespace rdbus;

using Code = rdbus::Data::Error::Code;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

static Slave getSlave()
{
    Slave slave;
    slave.id = 1;
    slave.name = "slave";
    slave.pollTimeMs = std::chrono::milliseconds( 100 );

    {
        Register reg;
        reg.address = 100;
        reg.byteOrder = { 0, 1, 2, 3 };
        reg.name = "Register_A";
        reg.type = rdbus::Type::Uint32;
        slave.inputRegisters.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 102;
        reg.byteOrder = { 0, 1, 2, 3, 4, 5, 6, 7 };
        reg.name = "Register_B";
        reg.type = rdbus::Type::Uint64;
        slave.inputRegisters.emplace_back( std::move( reg ) );
    }

    return slave;
}

static const Serial& getSerial()
{
    const static Serial serial = {
        .path = "/dev/path",
        .baudRate = 9600,
        .stopBitsCount = 2,
        .responseTimeout = std::chrono::milliseconds( 500 ),
        .lineTimeout = std::chrono::milliseconds( 10 ),
        .parity = Serial::Parity::Even
    };

    return serial;
}

// OS exceptions ---------------------------------------------------------------------------------------------------------------------------------------
TEST( TestCommunicator, TestInitOpenFail )
{
    auto os = std::make_unique< testing::NiceMock< OSMock > >();

    EXPECT_CALL( *os, open( _, _ ) ).WillOnce( Return( -1 ) );

    EXPECT_THROW( {
        communication::modbus::Communicator com( getSerial(), std::move( os ) );
    },
                  communication::OS::Exception );
}

TEST( TestCommunicator, TestInitGetAtrrFail )
{
    auto os = std::make_unique< testing::NiceMock< OSMock > >();

    EXPECT_CALL( *os, tcgetattr( _, _ ) ).WillOnce( Return( -1 ) );

    EXPECT_THROW( {
        communication::modbus::Communicator com( getSerial(), std::move( os ) );
    },
                  communication::OS::Exception );
}

TEST( TestCommunicator, TestInitFlushFail )
{
    auto os = std::make_unique< testing::NiceMock< OSMock > >();

    EXPECT_CALL( *os, tcflush( _, _ ) ).WillOnce( Return( -1 ) );

    EXPECT_THROW( {
        communication::modbus::Communicator com( getSerial(), std::move( os ) );
    },
                  communication::OS::Exception );
}

TEST( TestCommunicator, TestInitSetAttrFail )
{
    auto os = std::make_unique< testing::NiceMock< OSMock > >();

    EXPECT_CALL( *os, tcsetattr( _, _, _ ) ).WillOnce( Return( -1 ) );

    EXPECT_THROW( {
        communication::modbus::Communicator com( getSerial(), std::move( os ) );
    },
                  communication::OS::Exception );
}

TEST( TestCommunicator, TestFlushFail )
{
    auto os = std::make_unique< testing::NiceMock< OSMock > >();

    EXPECT_CALL( *os, tcflush( _, _ ) )
    .WillOnce( Return( 0 ) ) // During initialization
    .WillOnce( Return( -1 ) ); // Before sending data

    communication::modbus::Communicator com( getSerial(), std::move( os ) );

    const auto data = com.request( getSlave() );

    EXPECT_EQ( data.deviceName, "slave" );
    EXPECT_TRUE( data.fields.empty() );
    ASSERT_TRUE( data.error.has_value() );
    EXPECT_EQ( data.error->code, Code::OS );
    EXPECT_FALSE( data.error->what.empty() );
}

TEST( TestCommunicator, TestWriteFail )
{
    auto os = std::make_unique< testing::NiceMock< OSMock > >();

    EXPECT_CALL( *os, write( _, _, _ ) )
    .WillOnce( Return( -1 ) );

    communication::modbus::Communicator com( getSerial(), std::move( os ) );

    const auto data = com.request( getSlave() );

    EXPECT_EQ( data.deviceName, "slave" );
    EXPECT_TRUE( data.fields.empty() );
    ASSERT_TRUE( data.error.has_value() );
    EXPECT_EQ( data.error->code, Code::OS );
    EXPECT_FALSE( data.error->what.empty() );
}

TEST( TestCommunicator, TestInitialPollFail )
{
    auto os = std::make_unique< testing::NiceMock< OSMock > >();

    EXPECT_CALL( *os, poll( _, _, _ ) )
    .WillOnce( Return( 0 ) );

    communication::modbus::Communicator com( getSerial(), std::move( os ) );

    const auto data = com.request( getSlave() );

    EXPECT_EQ( data.deviceName, "slave" );
    EXPECT_TRUE( data.fields.empty() );
    ASSERT_TRUE( data.error.has_value() );
    EXPECT_EQ( data.error->code, Code::OS );
    EXPECT_FALSE( data.error->what.empty() );
}

TEST( TestCommunicator, TestReadFail )
{
    auto os = std::make_unique< testing::NiceMock< OSMock > >();

    EXPECT_CALL( *os, poll( _, _, _ ) )
    .WillOnce( Return( 1 ) );

    EXPECT_CALL( *os, read( _, _, _ ) )
    .WillOnce( Return( -1 ) );

    communication::modbus::Communicator com( getSerial(), std::move( os ) );

    const auto data = com.request( getSlave() );

    EXPECT_EQ( data.deviceName, "slave" );
    EXPECT_TRUE( data.fields.empty() );
    ASSERT_TRUE( data.error.has_value() );
    EXPECT_EQ( data.error->code, Code::OS );
    EXPECT_FALSE( data.error->what.empty() );
}

// Modbus exceptions ---------------------------------------------------------------------------------------------------------------------------------------
TEST( TestCommunicator, TestMBIllegalFunction )
{
    auto os = std::make_unique< testing::NiceMock< OSMock > >();

    EXPECT_CALL( *os, poll( _, _, _ ) )
    .WillOnce( Return( 1 ) )
    .WillRepeatedly( Return( 0 ) );

    EXPECT_CALL( *os, read( _, _, _ ) )
    .WillOnce( Invoke( []( int, void* buf, size_t length )
                       {
        const std::vector< uint8_t > rawResponse = { 0x01, 0x83, 0x01, 0x80, 0xf0 };
        std::memcpy( buf, rawResponse.data(), rawResponse.size() );
        return rawResponse.size(); } ) );

    communication::modbus::Communicator com( getSerial(), std::move( os ) );

    const auto data = com.request( getSlave() );

    EXPECT_EQ( data.deviceName, "slave" );
    EXPECT_TRUE( data.fields.empty() );
    ASSERT_TRUE( data.error.has_value() );
    EXPECT_EQ( data.error->code, Code::Modbus );
    EXPECT_FALSE( data.error->what.empty() );
}

TEST( TestCommunicator, TestMBSlaveDeviceFailure )
{
    auto os = std::make_unique< testing::NiceMock< OSMock > >();

    EXPECT_CALL( *os, poll( _, _, _ ) )
    .WillOnce( Return( 1 ) )
    .WillRepeatedly( Return( 0 ) );

    EXPECT_CALL( *os, read( _, _, _ ) )
    .WillOnce( Invoke( []( int, void* buf, size_t length )
                       {
        const std::vector< uint8_t > rawResponse = { 0x01, 0x83, 0x04, 0x40, 0xf3 };
        std::memcpy( buf, rawResponse.data(), rawResponse.size() );
        return rawResponse.size(); } ) );

    communication::modbus::Communicator com( getSerial(), std::move( os ) );

    const auto data = com.request( getSlave() );

    EXPECT_EQ( data.deviceName, "slave" );
    EXPECT_TRUE( data.fields.empty() );
    ASSERT_TRUE( data.error.has_value() );
    EXPECT_EQ( data.error->code, Code::Modbus );
    EXPECT_FALSE( data.error->what.empty() );
}

// Valid tests ---------------------------------------------------------------------------------------------------------------------------------------------
TEST( TestCommunicator, TestValid )
{
    auto os = std::make_unique< testing::NiceMock< OSMock > >();

    EXPECT_CALL( *os, poll( _, _, _ ) )
    .WillOnce( Return( 1 ) )
    .WillRepeatedly( Return( 0 ) );

    EXPECT_CALL( *os, read( _, _, _ ) )
    .WillOnce( Invoke( []( int, void* buf, size_t length )
                       {
        const std::vector< uint8_t > rawResponse = { 0x01, 0x03, 0x0c, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x4a, 0x13 };
        std::memcpy( buf, rawResponse.data(), rawResponse.size() );
        return rawResponse.size(); } ) );

    communication::modbus::Communicator com( getSerial(), std::move( os ) );

    const auto data = com.request( getSlave() );

    EXPECT_EQ( data.deviceName, "slave" );
    ASSERT_EQ( data.fields.size(), 2 );
    auto it = data.fields.begin();
    ASSERT_TRUE( it->value.has_value() );
    ASSERT_TRUE( std::holds_alternative< uint32_t >( it->value.value() ) );
    EXPECT_EQ( std::get< uint32_t >( *it->value ), 2 );
    EXPECT_EQ( it->name, "Register_A" );
    it++;
    ASSERT_TRUE( it->value.has_value() );
    ASSERT_TRUE( std::holds_alternative< uint64_t >( it->value.value() ) );
    EXPECT_EQ( std::get< uint64_t >( *it->value ), 5 );
    EXPECT_EQ( it->name, "Register_B" );
    ASSERT_FALSE( data.error.has_value() );
}
