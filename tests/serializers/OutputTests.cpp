#include "rdbus/config/Output.hpp"
#include "rdbus/config/exception.hpp"
#include "tests/utility.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <testDataDirectory.hpp>

const std::string testFilePath = TEST_DATA_DIR "/serializers/json_files/output/";
using namespace nlohmann;
using namespace rdbus;

TEST( TestOutput, TestDeserializationValidTCPIP )
{
    const auto path = testFilePath + "valid_tcpip.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Output output = jsonIn;

    ASSERT_TRUE( output.ip.has_value() );
    ASSERT_TRUE( output.port.has_value() );
    EXPECT_EQ( output.ip, "0.0.0.0" );
    EXPECT_EQ( output.port, 6000 );
    EXPECT_EQ( output.type, config::Output::TCP_IP );
}

TEST( TestOutput, TestDeserializationValidStdout )
{
    const auto path = testFilePath + "valid_stdout.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Output output = jsonIn;

    ASSERT_FALSE( output.ip.has_value() );
    ASSERT_FALSE( output.port.has_value() );
    EXPECT_EQ( output.type, config::Output::Stdout );
}

TEST( TestOutput, TestDeserializationNoPort )
{
    const auto path = testFilePath + "no_port.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Output output = jsonIn;
    },
                  config::ParseException );
}

TEST( TestOutput, TestDeserializationNoIp )
{
    const auto path = testFilePath + "no_ip.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Output output = jsonIn;
    },
                  config::ParseException );
}

TEST( TestOutput, TestDeserializationMixed )
{
    const auto path = testFilePath + "mixed.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Output output = jsonIn;
    },
                  config::ParseException );
}

TEST( TestOutput, TestDeserializationNoType )
{
    const auto path = testFilePath + "no_type.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Output output = jsonIn;
    },
                  config::ParseException );
}
