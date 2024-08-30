#include "rdbus/communication/nmea/Communicator.hpp"
#include "communication/OSMock.hpp"
#include "rdbus/Data.hpp"
#include "rdbus/config/Serial.hpp"
#include "rdbus/config/nmea/Sentence.hpp"
#include "gmock/gmock.h"
#include <chrono>
#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace rdbus::config::nmea;
using namespace rdbus::config;
using namespace rdbus;

using Code = rdbus::Data::Error::Code;

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;


static NMEA getNMEASettings()
{
    NMEA settings;

    std::list< Sentence > sentences;
    {
        Sentence sentence;
        sentence.id = "VHW";

        {
            Field field;
            field.type = Type::Uint64;
            field.name = "Degrees true";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::String;
            field.name = "True";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::Uint64;
            field.name = "Degrees magnetic";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::String;
            field.name = "Magnetic";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::Uint64;
            field.name = "STW (knots/h)";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::String;
            field.name = "Units";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::String;
            field.name = "More";
            sentence.fields.emplace_back( std::move( field ) );
        }

        sentences.emplace_back( std::move( sentence ) );
    }

    {
        Sentence sentence;
        sentence.id = "SDDBS";

        {
            Field field;
            field.type = Type::Double;
            field.name = "Depth";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::String;
            field.name = "Units";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::Double;
            field.name = "Depth";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::String;
            field.name = "Units";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::Double;
            field.name = "Depth";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::String;
            field.name = "Units";
            sentence.fields.emplace_back( std::move( field ) );
        }

        sentences.emplace_back( std::move( sentence ) );
    }

    {
        Sentence sentence;
        sentence.id = "VBW";

        {
            Field field;
            field.type = Type::Double;
            field.name = "Longitudinal water speed";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::Double;
            field.name = "Transverse water speed";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::String;
            field.name = "Data valid";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::Double;
            field.name = "Longitudinal ground speed";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::Double;
            field.name = "Transverse ground speed";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::String;
            field.name = "Data valid";
            sentence.fields.emplace_back( std::move( field ) );
        }

        sentences.emplace_back( std::move( sentence ) );
    }

    settings.withChecksum = true;
    settings.name = "NMEA Sensor";
    settings.sentences = std::move( sentences );

    return settings;
}

static const Serial& getSerial()
{
    const static Serial serial = {
        .path = "/dev/path",
        .baudRate = 4800,
        .stopBitsCount = 1,
        .responseTimeout = std::chrono::milliseconds( 500 ),
        .lineTimeout = std::chrono::milliseconds( 10 ),
        .parity = Serial::Parity::None
    };

    return serial;
}

TEST( TestCommunicatorNMEA, MergedResponse )
{
    auto os = std::make_unique< testing::NiceMock< OSMock > >();

    EXPECT_CALL( *os, poll( _, _, _ ) )
    .WillOnce( Return( 1 ) )
    .WillOnce( Return( 1 ) )
    .WillRepeatedly( Return( 0 ) );

    // Emulate a two part response
    EXPECT_CALL( *os, read( _, _, _ ) )
    .WillOnce( Invoke( []( int, void* buf, size_t length )
                       {
        const std::string rawResponse = "$VDVHW,55,T,43,M,22,N,11*27\r\n" // present in configuration
                                        "$VDVBW,55.42,,V,424.33,2321.22,V*49\r\n" // present in configuration
                                        "$VDRMC,181704,A,5209.6815,N,00643.0724,E,000.1,187.0,310109*12\r\n"
                                        "$VDWIMWV,214.8,R,0.1,K*57\r\n"
                                        "$VDSDMTW,26.8*75\r\n";

        std::memcpy( buf, rawResponse.data(), rawResponse.size() );
        return rawResponse.size(); } ) )

    .WillOnce( Invoke( []( int, void* buf, size_t length )
                       {

        const std::string rawResponse = "$VDSDDBK,1330.5,f,0405.5,M,0221.6*56\r\n"
                                        "$VDSDDBS,1330.5,f,0405.5,M,,*7B\r\n" // present in configuration
                                        "$VDSDDBT,1330.5,f,0405.5,M,0221.6*49\r\n";

        std::memcpy( buf, rawResponse.data(), rawResponse.size() );
        return rawResponse.size(); } ) );


    communication::nmea::Communicator com( getSerial(), std::move( os ) );

    const auto data = com.receive( getNMEASettings() );

    ASSERT_EQ( data.size(), 8 );

    auto it = data.begin();
    {
        ASSERT_TRUE( it->metadata.has_value() );
        EXPECT_EQ( it->metadata, "VHW" );
        EXPECT_EQ( it->deviceName, "NMEA Sensor" );
        EXPECT_FALSE( it->error.has_value() );
        ASSERT_EQ( it->fields.size(), 7 );

        auto fieldIt = it->fields.begin();
        EXPECT_EQ( fieldIt->name, "Degrees true" );
        EXPECT_EQ( fieldIt->type, Type::Uint64 );
        EXPECT_EQ( std::get< uint64_t >( fieldIt->value ), 55 );

        fieldIt++;
        EXPECT_EQ( fieldIt->name, "True" );
        EXPECT_EQ( fieldIt->type, Type::String );
        EXPECT_EQ( std::get< std::string >( fieldIt->value ), "T" );

        fieldIt++;
        EXPECT_EQ( fieldIt->name, "Degrees magnetic" );
        EXPECT_EQ( fieldIt->type, Type::Uint64 );
        EXPECT_EQ( std::get< uint64_t >( fieldIt->value ), 43 );

        fieldIt++;
        EXPECT_EQ( fieldIt->name, "Magnetic" );
        EXPECT_EQ( fieldIt->type, Type::String );
        EXPECT_EQ( std::get< std::string >( fieldIt->value ), "M" );

        fieldIt++;
        EXPECT_EQ( fieldIt->name, "STW (knots/h)" );
        EXPECT_EQ( fieldIt->type, Type::Uint64 );
        EXPECT_EQ( std::get< uint64_t >( fieldIt->value ), 22 );

        fieldIt++;
        EXPECT_EQ( fieldIt->name, "Units" );
        EXPECT_EQ( fieldIt->type, Type::String );
        EXPECT_EQ( std::get< std::string >( fieldIt->value ), "N" );

        fieldIt++;
        EXPECT_EQ( fieldIt->name, "More" );
        EXPECT_EQ( fieldIt->type, Type::String );
        EXPECT_EQ( std::get< std::string >( fieldIt->value ), "11" );
    }

    it++;
    {
        ASSERT_TRUE( it->metadata.has_value() );
        EXPECT_EQ( it->metadata, "VBW" );
        EXPECT_EQ( it->deviceName, "NMEA Sensor" );
        EXPECT_FALSE( it->error.has_value() );
        ASSERT_EQ( it->fields.size(), 6 );

        auto fieldIt = it->fields.begin();
        EXPECT_EQ( fieldIt->name, "Longitudinal water speed" );
        EXPECT_EQ( fieldIt->type, Type::Double );
        EXPECT_EQ( std::get< double >( fieldIt->value ), 55.42 );

        fieldIt++;
        EXPECT_EQ( fieldIt->name, "Transverse water speed" );
        EXPECT_EQ( fieldIt->type, Type::None );

        fieldIt++;
        EXPECT_EQ( fieldIt->name, "Data valid" );
        EXPECT_EQ( fieldIt->type, Type::String );
        EXPECT_EQ( std::get< std::string >( fieldIt->value ), "V" );

        fieldIt++;
        EXPECT_EQ( fieldIt->name, "Longitudinal ground speed" );
        EXPECT_EQ( fieldIt->type, Type::Double );
        EXPECT_EQ( std::get< double >( fieldIt->value ), 424.33 );

        fieldIt++;
        EXPECT_EQ( fieldIt->name, "Transverse ground speed" );
        EXPECT_EQ( fieldIt->type, Type::Double );
        EXPECT_EQ( std::get< double >( fieldIt->value ), 2321.22 );

        fieldIt++;
        EXPECT_EQ( fieldIt->name, "Data valid" );
        EXPECT_EQ( fieldIt->type, Type::String );
        EXPECT_EQ( std::get< std::string >( fieldIt->value ), "V" );
    }

    it++;
    ASSERT_FALSE( it->metadata.has_value() );
    EXPECT_EQ( it->deviceName, "NMEA Sensor" );
    ASSERT_TRUE( it->error.has_value() );
    EXPECT_EQ( it->error->code, Data::Error::NMEA );
    EXPECT_EQ( it->fields.size(), 0 );

    it++;
    ASSERT_FALSE( it->metadata.has_value() );
    EXPECT_EQ( it->deviceName, "NMEA Sensor" );
    ASSERT_TRUE( it->error.has_value() );
    EXPECT_EQ( it->error->code, Data::Error::NMEA );
    EXPECT_EQ( it->fields.size(), 0 );

    it++;
    ASSERT_FALSE( it->metadata.has_value() );
    EXPECT_EQ( it->deviceName, "NMEA Sensor" );
    ASSERT_TRUE( it->error.has_value() );
    EXPECT_EQ( it->error->code, Data::Error::NMEA );
    EXPECT_EQ( it->fields.size(), 0 );

    it++;
    ASSERT_FALSE( it->metadata.has_value() );
    EXPECT_EQ( it->deviceName, "NMEA Sensor" );
    ASSERT_TRUE( it->error.has_value() );
    EXPECT_EQ( it->error->code, Data::Error::NMEA );
    EXPECT_EQ( it->fields.size(), 0 );

    it++;
    {
        ASSERT_TRUE( it->metadata.has_value() );
        EXPECT_EQ( it->metadata, "SDDBS" );
        EXPECT_EQ( it->deviceName, "NMEA Sensor" );
        EXPECT_FALSE( it->error.has_value() );
        ASSERT_EQ( it->fields.size(), 6 );

        auto fieldIt = it->fields.begin();
        EXPECT_EQ( fieldIt->name, "Depth" );
        EXPECT_EQ( fieldIt->type, Type::Double );
        EXPECT_EQ( std::get< double >( fieldIt->value ), 1330.5 );

        fieldIt++;
        EXPECT_EQ( fieldIt->name, "Units" );
        EXPECT_EQ( fieldIt->type, Type::String );
        EXPECT_EQ( std::get< std::string >( fieldIt->value ), "f" );

        fieldIt++;
        EXPECT_EQ( fieldIt->name, "Depth" );
        EXPECT_EQ( fieldIt->type, Type::Double );
        EXPECT_EQ( std::get< double >( fieldIt->value ), 405.5 );

        fieldIt++;
        EXPECT_EQ( fieldIt->name, "Units" );
        EXPECT_EQ( fieldIt->type, Type::String );
        EXPECT_EQ( std::get< std::string >( fieldIt->value ), "M" );

        fieldIt++;
        EXPECT_EQ( fieldIt->name, "Depth" );
        EXPECT_EQ( fieldIt->type, Type::None );

        fieldIt++;
        EXPECT_EQ( fieldIt->name, "Units" );
        EXPECT_EQ( fieldIt->type, Type::None );
    }

    it++;
    ASSERT_FALSE( it->metadata.has_value() );
    EXPECT_EQ( it->deviceName, "NMEA Sensor" );
    ASSERT_TRUE( it->error.has_value() );
    EXPECT_EQ( it->error->code, Data::Error::NMEA );
    EXPECT_EQ( it->fields.size(), 0 );
}

TEST( TestCommunicatorNMEA, UnindentifiableResponse )
{
    auto os = std::make_unique< testing::NiceMock< OSMock > >();

    EXPECT_CALL( *os, poll( _, _, _ ) )
    .WillOnce( Return( 1 ) )
    .WillOnce( Return( 1 ) )
    .WillRepeatedly( Return( 0 ) );

    // Emulate a two part response
    EXPECT_CALL( *os, read( _, _, _ ) )
    .WillOnce( Invoke( []( int, void* buf, size_t length )
                       {
        const std::string rawResponse = "RIStCXYHRKPkisi";

        std::memcpy( buf, rawResponse.data(), rawResponse.size() );
        return rawResponse.size(); } ) )

    .WillOnce( Invoke( []( int, void* buf, size_t length )
                       {

        const std::string rawResponse = "YYd1Z5xl4vdUfo0hY8H86L0Yh0YEhTfo";

        std::memcpy( buf, rawResponse.data(), rawResponse.size() );
        return rawResponse.size(); } ) );


    communication::nmea::Communicator com( getSerial(), std::move( os ) );

    const auto data = com.receive( getNMEASettings() );

    ASSERT_EQ( data.size(), 1 );
    EXPECT_EQ( data.front().deviceName, "NMEA Sensor" );
    EXPECT_FALSE( data.front().metadata.has_value() );
    ASSERT_TRUE( data.front().error.has_value() );
    EXPECT_EQ( data.front().error->code, Data::Error::NMEA );
    EXPECT_FALSE( data.front().error->what.empty() );
    EXPECT_TRUE( data.front().fields.empty() );
}


TEST( TestCommunicatorNMEA, UnindentifiebleMessageBeforeValidOne )
{
    auto os = std::make_unique< testing::NiceMock< OSMock > >();

    EXPECT_CALL( *os, poll( _, _, _ ) )
    .WillOnce( Return( 1 ) )
    .WillOnce( Return( 1 ) )
    .WillRepeatedly( Return( 0 ) );

    // Emulate a two part response
    EXPECT_CALL( *os, read( _, _, _ ) )
    .WillOnce( Invoke( []( int, void* buf, size_t length )
                       {
        const std::string rawResponse = "8H86L0Yh0YEhTfo";

        std::memcpy( buf, rawResponse.data(), rawResponse.size() );
        return rawResponse.size(); } ) )

    .WillOnce( Invoke( []( int, void* buf, size_t length )
                       {

        const std::string rawResponse = "$VDVHW,55,T,43,M,22,N,11*27\r\n";

        std::memcpy( buf, rawResponse.data(), rawResponse.size() );
        return rawResponse.size(); } ) );


    communication::nmea::Communicator com( getSerial(), std::move( os ) );

    const auto data = com.receive( getNMEASettings() );

    ASSERT_EQ( data.size(), 1 );
    EXPECT_EQ( data.front().deviceName, "NMEA Sensor" );
    ASSERT_TRUE( data.front().metadata.has_value() );
    EXPECT_FALSE( data.front().error.has_value() );

    auto fieldIt = data.front().fields.begin();
    EXPECT_EQ( fieldIt->name, "Degrees true" );
    EXPECT_EQ( fieldIt->type, Type::Uint64 );
    EXPECT_EQ( std::get< uint64_t >( fieldIt->value ), 55 );

    fieldIt++;
    EXPECT_EQ( fieldIt->name, "True" );
    EXPECT_EQ( fieldIt->type, Type::String );
    EXPECT_EQ( std::get< std::string >( fieldIt->value ), "T" );

    fieldIt++;
    EXPECT_EQ( fieldIt->name, "Degrees magnetic" );
    EXPECT_EQ( fieldIt->type, Type::Uint64 );
    EXPECT_EQ( std::get< uint64_t >( fieldIt->value ), 43 );

    fieldIt++;
    EXPECT_EQ( fieldIt->name, "Magnetic" );
    EXPECT_EQ( fieldIt->type, Type::String );
    EXPECT_EQ( std::get< std::string >( fieldIt->value ), "M" );

    fieldIt++;
    EXPECT_EQ( fieldIt->name, "STW (knots/h)" );
    EXPECT_EQ( fieldIt->type, Type::Uint64 );
    EXPECT_EQ( std::get< uint64_t >( fieldIt->value ), 22 );

    fieldIt++;
    EXPECT_EQ( fieldIt->name, "Units" );
    EXPECT_EQ( fieldIt->type, Type::String );
    EXPECT_EQ( std::get< std::string >( fieldIt->value ), "N" );

    fieldIt++;
    EXPECT_EQ( fieldIt->name, "More" );
    EXPECT_EQ( fieldIt->type, Type::String );
    EXPECT_EQ( std::get< std::string >( fieldIt->value ), "11" );
}


TEST( TestCommunicatorNMEA, Timeout )
{
    auto os = std::make_unique< testing::NiceMock< OSMock > >();

    EXPECT_CALL( *os, poll( _, _, _ ) ).WillRepeatedly( Return( 0 ) );

    communication::nmea::Communicator com( getSerial(), std::move( os ) );

    const auto data = com.receive( getNMEASettings() );

    ASSERT_TRUE( data.empty() );
}

TEST( TestCommunicatorNMEA, OSFailure )
{
    auto os = std::make_unique< testing::NiceMock< OSMock > >();

    EXPECT_CALL( *os, poll( _, _, _ ) ).WillRepeatedly( Return( -1 ) );

    communication::nmea::Communicator com( getSerial(), std::move( os ) );

    const auto data = com.receive( getNMEASettings() );

    ASSERT_EQ( data.size(), 1 );
    EXPECT_EQ( data.front().deviceName, "NMEA Sensor" );
    EXPECT_FALSE( data.front().metadata.has_value() );
    ASSERT_TRUE( data.front().error.has_value() );
    EXPECT_EQ( data.front().error->code, Data::Error::OS );
    EXPECT_FALSE( data.front().error->what.empty() );
    EXPECT_TRUE( data.front().fields.empty() );
}
