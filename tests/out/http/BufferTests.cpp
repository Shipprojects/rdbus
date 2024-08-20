#include "rdbus/out/http/Buffer.hpp"
#include <chrono>
#include <gtest/gtest.h>

using namespace rdbus::out::http;

TEST( TestBuffer, TestAdd )
{
    Buffer buffer;

    {
        rdbus::Data entry;
        entry.deviceName = "Sensor1";
        buffer.add( { entry } );
    }

    {
        rdbus::Data entry;
        entry.deviceName = "Sensor2";
        buffer.add( { entry } );
    }

    {
        rdbus::Data entry;
        entry.deviceName = "Sensor3";
        buffer.add( { entry } );
    }

    ASSERT_EQ( buffer.data.size(), 3 );
    EXPECT_EQ( buffer.data[ 0 ].second.deviceName, "Sensor1" );
    EXPECT_EQ( buffer.data[ 1 ].second.deviceName, "Sensor2" );
    EXPECT_EQ( buffer.data[ 2 ].second.deviceName, "Sensor3" );
}

// While this test does not have testing macros, if the buffer auto-cleanup is not done
// correctly then this test will hang.
TEST( TestBuffer, TestSizeConstraint )
{
    Buffer buffer;
    int i = 0;
    while ( buffer.data.size() * sizeof( rdbus::Data ) < ( buffer.maxBytes + buffer.maxBytes / 10 ) )
    {
        std::list< rdbus::Data > entries;
        for ( int j = 0; j < 10; j++ )
        {
            rdbus::Data entry;
            entry.deviceName = "Sensor" + std::to_string( i++ );
            entries.emplace_back( std::move( entry ) );
        }

        buffer.add( entries );

        // If buffer started to auto-clean
        if ( buffer.data[ 0 ].second.deviceName != "Sensor0" )
        {
            break;
        }
    }
}

TEST( TestBuffer, TestParseFromBeginning )
{
    Buffer buffer;

    Buffer::TimePoint tp{};

    {
        rdbus::Data entry;
        entry.deviceName = "Sensor1";
        buffer.add( { entry } );
    }

    {
        rdbus::Data entry;
        entry.deviceName = "Sensor2";
        buffer.add( { entry } );
    }

    {
        rdbus::Data entry;
        entry.deviceName = "Sensor3";
        buffer.add( { entry } );
    }

    const auto& json = buffer.parseFrom( tp );
    ASSERT_EQ( json.size(), 3 );
    EXPECT_EQ( json[ 0 ].at( "device" ), "Sensor1" );
    EXPECT_EQ( json[ 1 ].at( "device" ), "Sensor2" );
    EXPECT_EQ( json[ 2 ].at( "device" ), "Sensor3" );
}

TEST( TestBuffer, TestParseFromMiddle )
{
    Buffer buffer;

    {
        rdbus::Data entry;
        entry.deviceName = "Sensor1";
        buffer.add( { entry } );
    }

    const auto& tp = std::chrono::system_clock::now();

    {
        rdbus::Data entry;
        entry.deviceName = "Sensor2";
        buffer.add( { entry } );
    }

    {
        rdbus::Data entry;
        entry.deviceName = "Sensor3";
        buffer.add( { entry } );
    }

    const auto& json = buffer.parseFrom( tp );
    ASSERT_EQ( json.size(), 2 );
    EXPECT_EQ( json[ 0 ].at( "device" ), "Sensor2" );
    EXPECT_EQ( json[ 1 ].at( "device" ), "Sensor3" );
}

TEST( TestBuffer, TestParseFromEnd )
{
    Buffer buffer;

    {
        rdbus::Data entry;
        entry.deviceName = "Sensor1";
        buffer.add( { entry } );
    }

    {
        rdbus::Data entry;
        entry.deviceName = "Sensor2";
        buffer.add( { entry } );
    }

    {
        rdbus::Data entry;
        entry.deviceName = "Sensor3";
        buffer.add( { entry } );
    }

    const auto& tp = std::chrono::system_clock::now();
    const auto& json = buffer.parseFrom( tp );
    ASSERT_EQ( json.size(), 0 );
}
