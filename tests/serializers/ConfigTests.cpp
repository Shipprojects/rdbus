#include "rdbus/config/Config.hpp"
#include "rdbus/config/Exception.hpp"
#include "tests/utility.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <testDataDirectory.hpp>

const std::string testFilePath = TEST_DATA_DIR "/serializers/json_files/config/";
using namespace nlohmann;
using namespace rdbus;

TEST( TestConfig, TestDeserializationInvalidRegisterSpacing )
{
    const auto path = testFilePath + "invalid_register_spacing.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, TestDeserializationDuplicateRegisterAddresses )
{
    const auto path = testFilePath + "duplicate_register_addresses.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, TestDeserializationDuplicateRegisterNames )
{
    const auto path = testFilePath + "duplicate_register_names.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, TestDeserializationDuplicateSlaveIDs )
{
    const auto path = testFilePath + "duplicate_slave_IDs.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, TestDeserializationDuplicateSlaveNames )
{
    const auto path = testFilePath + "duplicate_slave_names.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, TestDeserializationInvalidProtocol )
{
    const auto path = testFilePath + "invalid_protocol.json";

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

TEST( TestConfig, TestDeserializationValidModbus )
{
    const auto path = testFilePath + "valid_modbus.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Config config = jsonIn;

    EXPECT_EQ( config.protocol, "modbus" );
    EXPECT_EQ( config.serial.baudRate, 4800 );
    EXPECT_EQ( config.modbus.slaves.size(), 1 );
}

TEST( TestConfig, TestDeserializationValidNMEA )
{
    const auto path = testFilePath + "valid_nmea.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Config config = jsonIn;

    EXPECT_EQ( config.protocol, "nmea" );
    EXPECT_EQ( config.serial.baudRate, 4800 );
    EXPECT_EQ( config.nmea.sentences.size(), 2 );
    EXPECT_TRUE( config.nmea.withChecksum );
    EXPECT_EQ( config.nmea.name, "RPM reader" );
}

TEST( TestConfig, TestDeserializationNoChecksum )
{
    const auto path = testFilePath + "no_checksum.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, TestDeserializationDuplicateSentenceIDs )
{
    const auto path = testFilePath + "duplicate_sentence_IDs.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, TestDeserializationNoTalkerID )
{
    const auto path = testFilePath + "no_nmea_name.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Config config = jsonIn;
    },
                  config::ParseException );
}
