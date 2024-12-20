#include "rdbus/config/Exception.hpp"
#include "rdbus/config/wago/Module.hpp"
#include "tests/utility.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <testDataDirectory.hpp>

const std::string testFilePath = TEST_DATA_DIR "/serializers/json_files/wago/module/";
using namespace nlohmann;
using namespace rdbus::config::wago;
using namespace rdbus;

TEST( TestModule, Valid )
{
    const auto path = testFilePath + "valid.json";

    const auto jsonIn = getJsonFromPath( path );
    const Module module = jsonIn;

    EXPECT_EQ( module.name, "Sensors" );
    EXPECT_EQ( module.pollTimeMs, Module::Millis( 150 ) );
    EXPECT_EQ( module.offset, 12 );
    EXPECT_EQ( module.instances.size(), 2 );
    EXPECT_EQ( module.instances.begin()->name, "sensor_1" );
    EXPECT_EQ( module.instances.begin()->min, 0 );
    EXPECT_EQ( module.instances.begin()->max, 10 );
    EXPECT_EQ( std::next( module.instances.begin() )->name, "sensor_2" );
    EXPECT_EQ( std::next( module.instances.begin() )->min, -100 );
    EXPECT_EQ( std::next( module.instances.begin() )->max, 100 );
}

TEST( TestModule, ThreeInstances )
{
    const auto path = testFilePath + "3_instances.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const Module module = jsonIn;
    },
                  config::ParseException );
}

TEST( TestModule, FiveInstances )
{
    const auto path = testFilePath + "5_instances.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const Module module = jsonIn;
    },
                  config::ParseException );
}

TEST( TestModule, EmptyInstances )
{
    const auto path = testFilePath + "empty_instances.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const Module module = jsonIn;
    },
                  config::ParseException );
}

TEST( TestModule, NegativeOffset )
{
    const auto path = testFilePath + "negative_offset.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const Module module = jsonIn;
    },
                  config::ParseException );
}

TEST( TestModule, NoOffset )
{
    const auto path = testFilePath + "no_offset.json";

    const auto jsonIn = getJsonFromPath( path );
    const Module module = jsonIn;

    EXPECT_EQ( module.name, "Sensors" );
    EXPECT_EQ( module.pollTimeMs, Module::Millis( 150 ) );
    EXPECT_EQ( module.offset, Module::defaultOffset );
    EXPECT_EQ( module.instances.size(), 1 );
    EXPECT_EQ( module.instances.begin()->name, "sensor_2" );
    EXPECT_EQ( module.instances.begin()->min, 0 );
    EXPECT_EQ( module.instances.begin()->max, 10 );
}

TEST( TestModule, NoInstances )
{
    const auto path = testFilePath + "no_poll_pause.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const Module module = jsonIn;
    },
                  config::ParseException );
}

TEST( TestModule, NoName )
{
    const auto path = testFilePath + "no_poll_pause.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const Module module = jsonIn;
    },
                  config::ParseException );
}

TEST( TestModule, NoPollTime )
{
    const auto path = testFilePath + "no_poll_pause.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const Module module = jsonIn;
    },
                  config::ParseException );
}
