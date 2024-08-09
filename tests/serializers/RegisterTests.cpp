#include "config/Register.hpp"
#include "config/exception.hpp"
#include "tests/utility.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <testDataDirectory.hpp>

const std::string testFilePath = TEST_DATA_DIR "/serializers/json_files/register/";
using namespace nlohmann;

TEST( TestRegister, TestDeserializationBigEndian )
{
    const auto path = testFilePath + "big_endian_data_order.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Register reg = jsonIn;

    EXPECT_EQ( reg.name, "REGISTER_A" );
    EXPECT_EQ( reg.address, 40000 );

    const decltype( reg.byteOrder ) need = { 0, 1, 2, 3 };
    EXPECT_EQ( reg.byteOrder, need );
}

TEST( TestRegister, TestDeserializationMixedOrder )
{
    const auto path = testFilePath + "mixed_data_order.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Register reg = jsonIn;

    EXPECT_EQ( reg.name, "" );
    EXPECT_EQ( reg.address, 10 );

    const decltype( reg.byteOrder ) need = { 2, 1, 6, 0, 4, 5, 3, 7 };
    EXPECT_EQ( reg.byteOrder, need );
}

TEST( TestRegister, TestDeserializationNoAddress )
{
    const auto path = testFilePath + "no_address.json";

    const auto jsonIn = getJsonFromPath( path );
    EXPECT_THROW( {
        config::Register reg = jsonIn;
    },
                  config::ParseException );
}

TEST( TestRegister, TestDeserializationSmallEndian )
{
    const auto path = testFilePath + "small_endian_data_order.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Register reg = jsonIn;

    EXPECT_EQ( reg.name, "" );
    EXPECT_EQ( reg.address, 10 );

    const decltype( reg.byteOrder ) need = { 5, 4, 3, 2, 1, 0 };
    EXPECT_EQ( reg.byteOrder, need );
}
