#include "rdbus/config/Slave.hpp"
#include "rdbus/config/exception.hpp"
#include "tests/utility.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <testDataDirectory.hpp>

const std::string testFilePath = TEST_DATA_DIR "/serializers/json_files/slave/";
using namespace nlohmann;
using namespace rdbus;

TEST( TestSlave, TestDeserializationNoRegisters )
{
    const auto path = testFilePath + "no_registers.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const config::Slave slave = jsonIn;
    },
                  config::ParseException );
}

TEST( TestSlave, TestDeserializationNoAddress )
{
    const auto path = testFilePath + "no_address.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const config::Slave slave = jsonIn;
    },
                  config::ParseException );
}

TEST( TestSlave, TestDeserializationNoPollPause )
{
    const auto path = testFilePath + "no_poll_pause.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Slave slave = jsonIn;
    },
                  config::ParseException );
}

TEST( TestSlave, TestDeserializationValid )
{
    const auto path = testFilePath + "valid.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Slave slave = jsonIn;

    EXPECT_EQ( slave.name, "Sensor" );
    EXPECT_EQ( slave.address, 1 );
    EXPECT_EQ( slave.pollTimeMs, config::Slave::Millis( 150 ) );
    EXPECT_EQ( slave.registers.size(), 2 );
}
