#include "rdbus/config/Output.hpp"
#include "rdbus/config/exception.hpp"
#include "tests/utility.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <testDataDirectory.hpp>

const std::string testFilePath = TEST_DATA_DIR "/serializers/json_files/output/";
using namespace nlohmann;
using namespace rdbus;

TEST( TestOutput, TestDeserializationFile )
{
    const auto path = testFilePath + "file.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Output output = jsonIn;

    EXPECT_EQ( output.type, config::Output::File );
    EXPECT_EQ( output.filePath.value(), "relative/path/to/file" );
    EXPECT_FALSE( output.ipPortPair.has_value() );
}

TEST( TestOutput, TestDeserializationIP )
{
    const auto path = testFilePath + "tcp_ip.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Output output = jsonIn;

    EXPECT_EQ( output.type, config::Output::TCP_IP );
    EXPECT_EQ( output.ipPortPair->first, "0.0.0.0" );
    EXPECT_EQ( output.ipPortPair->second, 6000 );
    EXPECT_FALSE( output.filePath.has_value() );
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
