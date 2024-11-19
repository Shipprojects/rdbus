#include "rdbus/config/Exception.hpp"
#include "rdbus/config/wago/Instance.hpp"
#include "tests/utility.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <testDataDirectory.hpp>

const std::string testFilePath = TEST_DATA_DIR "/serializers/json_files/wago/instance/";
using namespace nlohmann;
using namespace rdbus;

TEST( TestInstance, Valid )
{
    const auto path = testFilePath + "valid.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::wago::Instance instance = jsonIn;

    EXPECT_EQ( instance.name, "instance" );
    EXPECT_EQ( instance.min, 0 );
    EXPECT_EQ( instance.max, 10 );
}

TEST( TestInstance, NoMax )
{
    const auto path = testFilePath + "no_max.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::wago::Instance output = jsonIn;
    },
                  config::ParseException );
}

TEST( TestInstance, NoMin )
{
    const auto path = testFilePath + "no_min.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::wago::Instance output = jsonIn;
    },
                  config::ParseException );
}

TEST( TestInstance, NoName )
{
    const auto path = testFilePath + "no_name.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::wago::Instance output = jsonIn;
    },
                  config::ParseException );
}


TEST( TestInstance, InvalidRange )
{
    const auto path = testFilePath + "invalid_range.json";
    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::wago::Instance output = jsonIn;
    },
                  config::ParseException );
}
