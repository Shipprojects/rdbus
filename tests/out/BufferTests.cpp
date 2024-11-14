#include "rdbus/out/Buffer.hpp"
#include "rdbus/processing/Base.hpp"
#include "rdbus/processing/limits/Data.hpp"
#include <chrono>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace rdbus::out;

TEST( TestBuffer, Add )
{
    Buffer< rdbus::Data > buffer;

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
TEST( TestBuffer, SizeConstraint )
{
    Buffer< rdbus::Data > buffer;
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

TEST( TestBuffer, ParseFromBeginning )
{
    Buffer< rdbus::Data > buffer;

    BufferTimePoint tp{};

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

TEST( TestBuffer, ParseFromMiddle )
{
    Buffer< rdbus::Data > buffer;

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

TEST( TestBuffer, ParseFromEnd )
{
    Buffer< rdbus::Data > buffer;

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

TEST( TestBuffer, BufferTypeSingle )
{
    using namespace rdbus::processing;
    using namespace nlohmann::literals;

    Buffer< ProcessingData > buffer( BufferType::Single, Name::Limits );

    {
        std::list< ProcessingData > list;
        {
            limits::Data data;
            data.deviceName = "Sensor_1";
            data.instanceLimits = {
                { "instance_1",
                  0,
                  100 },
                { "instance_2",
                  100,
                  300 }
            };
            list.emplace_back( std::make_shared< rdbus::processing::limits::Data >( std::move( data ) ) );
        }

        {
            limits::Data data;
            data.deviceName = "Sensor_2";
            data.instanceLimits = {
                { "instance_3",
                  -5,
                  1 },
                { "instance_4",
                  3,
                  3 }
            };
            list.emplace_back( std::make_shared< rdbus::processing::limits::Data >( std::move( data ) ) );
        }

        buffer.add( list );
    }

    {
        std::list< ProcessingData > list;
        {
            limits::Data data;
            data.deviceName = "Sensor_1";
            data.instanceLimits = {
                { "instance_1",
                  3,
                  100 },
                { "instance_2",
                  100,
                  301 }
            };
            list.emplace_back( std::make_shared< rdbus::processing::limits::Data >( std::move( data ) ) );
        }

        buffer.add( list );
    }

    using TimePoint = std::chrono::time_point< std::chrono::system_clock >;
    const auto& j = buffer.parseFrom( TimePoint() );

    EXPECT_EQ( j, R"([
        {
            "device": "Sensor_2",
            "fields": [
                {
                    "name": "instance_3",
                    "min": -5,
                    "max": 1
                },
                {
                    "name": "instance_4",
                    "min": 3,
                    "max": 3
                }
            ]
        },
        {
            "device": "Sensor_1",
            "fields": [
                {
                    "name": "instance_1",
                    "min": 3,
                    "max": 100
                },
                {
                    "name": "instance_2",
                    "min": 100,
                    "max": 301
                }
            ]
        }])"_json );
}
