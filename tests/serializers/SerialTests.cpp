#include "rdbus/config/Serial.hpp"
#include "rdbus/config/exception.hpp"
#include "tests/utility.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <testDataDirectory.hpp>

const std::string testFilePath = TEST_DATA_DIR "/serializers/json_files/serial/";
using namespace nlohmann;
using namespace rdbus;

TEST( TestSerial, TestDeserializationInvalidPath )
{
    const auto path = testFilePath + "invalid_path.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const config::Serial serial = jsonIn;
    },
                  config::ParseException );
}

TEST( TestSerial, TestDeserializationMissingField )
{
    const auto path = testFilePath + "missing_field.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const config::Serial serial = jsonIn;
    },
                  config::ParseException );
}

TEST( TestSerial, TestDeserializationTooManyStopBits )
{
    const auto path = testFilePath + "too_many_stop_bits.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const config::Serial serial = jsonIn;
    },
                  config::ParseException );
}

TEST( TestSerial, TestDeserializationValid )
{
    const auto path = testFilePath + "valid.json";

    const auto jsonIn = getJsonFromPath( path );

    const config::Serial serial = jsonIn;

    EXPECT_EQ( serial.baudRate, 9600 );
    EXPECT_EQ( serial.parity, true );
    EXPECT_EQ( serial.path, "/dev/ttyS1" );
    EXPECT_EQ( serial.stopBitsCount, 2 );
}
