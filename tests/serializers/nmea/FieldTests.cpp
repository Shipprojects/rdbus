#include "rdbus/Data.hpp"
#include "rdbus/config/exception.hpp"
#include "rdbus/config/nmea/Field.hpp"
#include "tests/utility.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <testDataDirectory.hpp>

const std::string testFilePath = TEST_DATA_DIR "/serializers/json_files/nmea/field/";
using namespace nlohmann;
using namespace rdbus;

TEST( TestField, TestDeserializationValid )
{
    const auto path = testFilePath + "valid.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::nmea::Field field = jsonIn;

    EXPECT_EQ( field.name, "Time Difference A" );
    EXPECT_EQ( field.type, rdbus::Type::Double );
}

TEST( TestField, TestDeserializationNoName )
{
    const auto path = testFilePath + "no_name.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::nmea::Field field = jsonIn;
    },
                  config::ParseException );
}

TEST( TestField, TestDeserializationNoType )
{
    const auto path = testFilePath + "no_type.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::nmea::Field field = jsonIn;
    },
                  config::ParseException );
}

TEST( TestField, TestDeserializationInvalidType )
{
    const auto path = testFilePath + "invalid_type.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::nmea::Field field = jsonIn;
    },
                  config::ParseException );
}
