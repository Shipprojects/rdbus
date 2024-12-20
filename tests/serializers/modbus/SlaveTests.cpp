#include "rdbus/config/Exception.hpp"
#include "rdbus/config/modbus/Slave.hpp"
#include "tests/utility.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <testDataDirectory.hpp>

const std::string testFilePath = TEST_DATA_DIR "/serializers/json_files/modbus/slave/";
using namespace nlohmann;
using namespace rdbus::config::modbus;
using namespace rdbus;

TEST( TestSlave, TestDeserializationNoRegisters )
{
    const auto path = testFilePath + "no_registers.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const Slave slave = jsonIn;
    },
                  config::ParseException );
}

TEST( TestSlave, TestDeserializationNoID )
{
    const auto path = testFilePath + "no_ID.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const Slave slave = jsonIn;
    },
                  config::ParseException );
}

TEST( TestSlave, TestDeserializationNoPollPause )
{
    const auto path = testFilePath + "no_poll_pause.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        Slave slave = jsonIn;
    },
                  config::ParseException );
}

TEST( TestSlave, TestDeserializationValid )
{
    const auto path = testFilePath + "valid.json";

    const auto jsonIn = getJsonFromPath( path );
    const Slave slave = jsonIn;

    EXPECT_EQ( slave.name, "Sensor" );
    EXPECT_EQ( slave.id, 1 );
    EXPECT_EQ( slave.pollTimeMs, Slave::Millis( 150 ) );
    EXPECT_EQ( slave.outputRegisters.size(), 1 );
    EXPECT_EQ( slave.inputRegisters.size(), 2 );
}
