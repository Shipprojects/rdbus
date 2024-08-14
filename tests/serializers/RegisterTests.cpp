#include "rdbus/Data.hpp"
#include "rdbus/config/Register.hpp"
#include "rdbus/config/exception.hpp"
#include "tests/utility.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <testDataDirectory.hpp>

const std::string testFilePath = TEST_DATA_DIR "/serializers/json_files/register/";
using namespace nlohmann;
using namespace rdbus;

TEST( TestRegister, TestDeserializationF64A )
{
    const auto path = testFilePath + "F64A.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Register reg = jsonIn;

    EXPECT_EQ( reg.name, "A" );
    EXPECT_EQ( reg.address, 1 );

    const decltype( reg.byteOrder ) need = { 0, 1, 2, 3, 4, 5, 6, 7 };
    EXPECT_EQ( reg.byteOrder, need );
    EXPECT_EQ( reg.type, Type::Double );
}

TEST( TestRegister, TestDeserializationU64B )
{
    const auto path = testFilePath + "U64B.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Register reg = jsonIn;

    EXPECT_EQ( reg.name, "B" );
    EXPECT_EQ( reg.address, 2 );

    const decltype( reg.byteOrder ) need = { 7, 6, 5, 4, 3, 2, 1, 0 };
    EXPECT_EQ( reg.byteOrder, need );
    EXPECT_EQ( reg.type, Type::Uint64 );
}

TEST( TestRegister, TestDeserializationS16A )
{
    const auto path = testFilePath + "S16A.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Register reg = jsonIn;

    EXPECT_EQ( reg.name, "C" );
    EXPECT_EQ( reg.address, 3 );

    const decltype( reg.byteOrder ) need = { 0, 1 };
    EXPECT_EQ( reg.byteOrder, need );
    EXPECT_EQ( reg.type, Type::Int16 );
}

TEST( TestRegister, TestDeserializationF32B )
{
    const auto path = testFilePath + "F32B.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Register reg = jsonIn;

    EXPECT_EQ( reg.name, "REGISTER_A" );
    EXPECT_EQ( reg.address, 92 );

    const decltype( reg.byteOrder ) need = { 1, 0, 3, 2 };
    EXPECT_EQ( reg.byteOrder, need );
    EXPECT_EQ( reg.type, Type::Float );
}

TEST( TestRegister, TestDeserializationS32C )
{
    const auto path = testFilePath + "S32C.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Register reg = jsonIn;

    EXPECT_EQ( reg.name, "REGISTER_A" );
    EXPECT_EQ( reg.address, 7 );

    const decltype( reg.byteOrder ) need = { 2, 3, 0, 1 };
    EXPECT_EQ( reg.byteOrder, need );
    EXPECT_EQ( reg.type, Type::Int32 );
}

TEST( TestRegister, TestDeserializationU32A )
{
    const auto path = testFilePath + "U32A.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Register reg = jsonIn;

    EXPECT_EQ( reg.name, "REGISTER_A" );
    EXPECT_EQ( reg.address, 5 );

    const decltype( reg.byteOrder ) need = { 0, 1, 2, 3 };
    EXPECT_EQ( reg.byteOrder, need );
    EXPECT_EQ( reg.type, Type::Uint32 );
}

TEST( TestRegister, TestDeserializationNoTypeNoOrder )
{
    const auto path = testFilePath + "no_type_no_order.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Register reg = jsonIn;
    },
                  config::ParseException );
}

TEST( TestRegister, TestDeserializationMixedOrder )
{
    const auto path = testFilePath + "mixed_data_order.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Register reg = jsonIn;

    EXPECT_EQ( reg.name, "REGISTER" );
    EXPECT_EQ( reg.address, 10 );

    const decltype( reg.byteOrder ) need = { 2, 1, 6, 0, 4, 5, 3, 7 };
    EXPECT_EQ( reg.byteOrder, need );
    EXPECT_EQ( reg.type, Type::Blob );
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
    const auto path = testFilePath + "no_name.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Register reg = jsonIn;
    },
                  config::ParseException );
}
