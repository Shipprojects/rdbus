#include "rdbus/config/Exception.hpp"
#include "rdbus/config/nmea/Sentence.hpp"
#include "tests/utility.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <testDataDirectory.hpp>

const std::string testFilePath = TEST_DATA_DIR "/serializers/json_files/nmea/sentence/";
using namespace nlohmann;
using namespace rdbus;

TEST( TestSentence, TestDeserializationValid )
{
    const auto path = testFilePath + "valid.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::nmea::Sentence sentence = jsonIn;

    EXPECT_EQ( sentence.id, "TIM" );
    ASSERT_EQ( sentence.fields.size(), 2 );
    EXPECT_EQ( sentence.fields.begin()->name, "Time Difference A" );
    EXPECT_EQ( std::next( sentence.fields.begin() )->name, "Time Difference B" );
}

TEST( TestSentence, TestDeserializationNoId )
{
    const auto path = testFilePath + "no_name.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::nmea::Field field = jsonIn;
    },
                  config::ParseException );
}

TEST( TestSentence, TestDeserializationNoFields )
{
    const auto path = testFilePath + "no_type.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::nmea::Field field = jsonIn;
    },
                  config::ParseException );
}

TEST( TestOutput, TestDeserializationEmptyFields )
{
    const auto path = testFilePath + "invalid_type.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::nmea::Field field = jsonIn;
    },
                  config::ParseException );
}

TEST( TestOutput, TestDeserializationDuplicateFieldNames )
{
    const auto path = testFilePath + "duplicate_field_names.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::nmea::Field field = jsonIn;
    },
                  config::ParseException );
}
