#include "config/Config.hpp"
#include "config/exception.hpp"
#include "tests/utility.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <testDataDirectory.hpp>

const std::string testFilePath = TEST_DATA_DIR "/serializers/json_files/config/";
using namespace nlohmann;

TEST( TestConfig, TestDeserializationInvalidProtocol )
{
    const auto path = testFilePath + "invalid_protocol.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, TestDeserializationNoOutput )
{
    const auto path = testFilePath + "no_output.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, TestDeserializationNoSerial )
{
    const auto path = testFilePath + "no_serial.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, TestDeserializationNoSlaves )
{
    const auto path = testFilePath + "no_slaves.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, TestDeserializationValid )
{
    const auto path = testFilePath + "valid.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Config config = jsonIn;

    EXPECT_EQ( config.protocol, "modbus" );
    EXPECT_EQ( config.output.filePath, "/path/to/output" );
    EXPECT_EQ( config.serial.baudRate, 4800 );
    EXPECT_EQ( config.slaves.size(), 1 );
}
