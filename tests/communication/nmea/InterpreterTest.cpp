#include "rdbus/communication/nmea/Interpreter.hpp"
#include "rdbus/Data.hpp"
#include "rdbus/communication/Interpreter.hpp"
#include "rdbus/communication/nmea/Response.hpp"
#include "rdbus/config/nmea/Sentence.hpp"
#include <MB/modbusResponse.hpp>
#include <chrono>
#include <cstdint>
#include <gtest/gtest.h>

using namespace rdbus;
using namespace rdbus::communication::nmea;
using namespace rdbus::config::nmea;
using namespace communication::nmea::interpreter;

static std::list< Sentence > getSentences()
{
    std::list< Sentence > sentences;
    {
        Sentence sentence;
        sentence.id = "MWV";

        {
            Field field;
            field.type = Type::Uint64;
            field.name = "Wing angle";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::String;
            field.name = "Reference";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::Double;
            field.name = "Wind speed";
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
            field.type = Type::Uint64;
            field.name = "A";
            sentence.fields.emplace_back( std::move( field ) );
        }

        sentences.emplace_back( std::move( sentence ) );
    }

    {
        Sentence sentence;
        sentence.id = "RMC";

        {
            Field field;
            field.type = Type::Uint64;
            field.name = "Current time in U.T.C";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::String;
            field.name = "GPS Status";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::Double;
            field.name = "Latitude";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::String;
            field.name = "N/S";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::Double;
            field.name = "Longitude";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::String;
            field.name = "E/W";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::Double;
            field.name = "SOG";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::Double;
            field.name = "COG";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::String;
            field.name = "Date";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::Uint64;
            field.name = "More_1";
            sentence.fields.emplace_back( std::move( field ) );
        }

        {
            Field field;
            field.type = Type::Uint64;
            field.name = "More_2";
            sentence.fields.emplace_back( std::move( field ) );
        }

        sentences.emplace_back( std::move( sentence ) );
    }

    return sentences;
}

TEST( TestNMEAInterpreter, TestParse )
{
    const std::string representation = "$GPRMC,181704,A,5209.6815,N,00643.0724,E,000.1,187.0,310109,,*24\r\n";
    const std::vector< uint8_t > input( representation.begin(), representation.end() );

    const Response response( input );

    auto sentences = getSentences();

    const auto& now = std::chrono::system_clock::now();
    const auto& fields = communication::nmea::interpreter::parse( response, sentences, now );

    ASSERT_EQ( fields.size(), 11 );

    auto it = fields.begin();
    EXPECT_EQ( it->name, "Current time in U.T.C" );
    EXPECT_EQ( it->type, Type::Uint64 );
    EXPECT_EQ( std::get< uint64_t >( it->value ), 181704 );
    EXPECT_EQ( it->timestamp, now );

    it++;
    EXPECT_EQ( it->name, "GPS Status" );
    EXPECT_EQ( it->type, Type::String );
    EXPECT_EQ( std::get< std::string >( it->value ), "A" );
    EXPECT_EQ( it->timestamp, now );

    it++;
    EXPECT_EQ( it->name, "Latitude" );
    EXPECT_EQ( it->type, Type::Double );
    EXPECT_EQ( std::get< double >( it->value ), 5209.6815 );
    EXPECT_EQ( it->timestamp, now );

    it++;
    EXPECT_EQ( it->name, "N/S" );
    EXPECT_EQ( it->type, Type::String );
    EXPECT_EQ( std::get< std::string >( it->value ), "N" );
    EXPECT_EQ( it->timestamp, now );

    it++;
    EXPECT_EQ( it->name, "Longitude" );
    EXPECT_EQ( it->type, Type::Double );
    EXPECT_EQ( std::get< double >( it->value ), 00643.0724 );
    EXPECT_EQ( it->timestamp, now );

    it++;
    EXPECT_EQ( it->name, "E/W" );
    EXPECT_EQ( it->type, Type::String );
    EXPECT_EQ( std::get< std::string >( it->value ), "E" );
    EXPECT_EQ( it->timestamp, now );

    it++;
    EXPECT_EQ( it->name, "SOG" );
    EXPECT_EQ( it->type, Type::Double );
    EXPECT_EQ( std::get< double >( it->value ), 000.1 );
    EXPECT_EQ( it->timestamp, now );


    it++;
    EXPECT_EQ( it->name, "COG" );
    EXPECT_EQ( it->type, Type::Double );
    EXPECT_EQ( std::get< double >( it->value ), 187.0 );
    EXPECT_EQ( it->timestamp, now );


    it++;
    EXPECT_EQ( it->name, "Date" );
    EXPECT_EQ( it->type, Type::String );
    EXPECT_EQ( std::get< std::string >( it->value ), "310109" );
    EXPECT_EQ( it->timestamp, now );

    it++;
    EXPECT_EQ( it->name, "More_1" );
    EXPECT_EQ( it->type, Type::None );
    EXPECT_EQ( it->timestamp, now );

    it++;
    EXPECT_EQ( it->name, "More_2" );
    EXPECT_EQ( it->type, Type::None );
    EXPECT_EQ( it->timestamp, now );
}

TEST( TestNMEAInterpreter, TestConfigMismatch )
{
    // There are more fields in incoming message than there are in sentence configuration
    const std::string representation = "$GPRMC,181704,A,5209.6815,N,00643.0724,E,000.1,187.0,310109,,,*24\r\n";
    const std::vector< uint8_t > input( representation.begin(), representation.end() );

    const Response response( input );

    auto sentences = getSentences();

    const auto& now = std::chrono::system_clock::now();
    EXPECT_THROW( {
        const auto& fields = communication::nmea::interpreter::parse( response, sentences, now );
    },
                  rdbus::Exception );
}

TEST( TestNMEAInterpreter, TestSingleSplit )
{
    const std::string representation = "$VPMWV,120,T,30,K,A*ab\r\n";
    const std::vector< uint8_t > data( representation.begin(), representation.end() );

    const auto& split = communication::nmea::interpreter::split( data );
    ASSERT_EQ( split.size(), 1 );
    EXPECT_EQ( split.front().size(), representation.size() );
}

TEST( TestNMEAInterpreter, TestMultipleSplit )
{
    const std::string representation = "$VPMWV,120,T,30,K,A*ab\r\n$VPMWV,1,T,2,,*a1\r\n$VPMWV,,,35,K,A*42\r\n";
    const std::vector< uint8_t > data( representation.begin(), representation.end() );

    const auto& split = communication::nmea::interpreter::split( data );
    ASSERT_EQ( split.size(), 3 );
    auto it = split.begin();
    EXPECT_EQ( it->size(), 24 );
    it++;
    EXPECT_EQ( it->size(), 19 );
    it++;
    EXPECT_EQ( it->size(), 20 );
}

TEST( TestNMEAInterpreter, TestSplitFromMiddle )
{
    // A case where message gets read from the middle
    const std::string representation = "MWV,120,T,30,K,A*ab\r\n$VPMWV,1,T,2,,*a1\r\n$VPMWV,,,35,K,A*42\r\n";
    const std::vector< uint8_t > data( representation.begin(), representation.end() );

    const auto& split = communication::nmea::interpreter::split( data );
    ASSERT_EQ( split.size(), 2 );
    auto it = split.begin();
    EXPECT_EQ( it->size(), 19 );
    it++;
    EXPECT_EQ( it->size(), 20 );
}

TEST( TestNMEAInterpreter, TestConfigInaproppriateData )
{
    // String instead of uint
    {
        const std::string representation = "$GPRMC,asdawra,A,5209.6815,N,00643.0724,E,000.1,187.0,310109,,*24\r\n";
        const std::vector< uint8_t > input( representation.begin(), representation.end() );

        const Response response( input );

        auto sentences = getSentences();

        const auto& now = std::chrono::system_clock::now();

        EXPECT_THROW( communication::nmea::interpreter::parse( response, sentences, now ), communication::interpreter::Exception );
    }

    // String instead of float
    {
        const std::string representation = "$GPRMC,181004,A,asdzxc,N,00643.0724,E,000.1,187.0,310109,,*24\r\n";
        const std::vector< uint8_t > input( representation.begin(), representation.end() );

        const Response response( input );

        auto sentences = getSentences();

        const auto& now = std::chrono::system_clock::now();

        EXPECT_THROW( communication::nmea::interpreter::parse( response, sentences, now ), communication::interpreter::Exception );
    }
}
