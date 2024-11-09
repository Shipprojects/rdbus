#include "rdbus/config/Exception.hpp"
#include "rdbus/config/Address.hpp"
#include "tests/utility.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <testDataDirectory.hpp>

const std::string testFilePath = TEST_DATA_DIR "/serializers/json_files/address/";
using namespace nlohmann;
using namespace rdbus;

TEST( TestAddress, TestDeserializationValidTCPIP )
{
    const auto path = testFilePath + "valid.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Address address = jsonIn;

    EXPECT_EQ( address.ip, "192.168.1.0" );
    EXPECT_EQ( address.port, 6060 );
}

TEST( TestAddress, TestDeserializationNoPort )
{
    const auto path = testFilePath + "no_port.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Address output = jsonIn;
    },
                  config::ParseException );
}

TEST( TestAddress, TestDeserializationNoIp )
{
    const auto path = testFilePath + "no_ip.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Address output = jsonIn;
    },
                  config::ParseException );
}
