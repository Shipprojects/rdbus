#include "rdbus/config/Config.hpp"
#include "rdbus/config/Exception.hpp"
#include "tests/utility.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <testDataDirectory.hpp>

const std::string testFilePath = TEST_DATA_DIR "/serializers/json_files/config/";
using namespace nlohmann;
using namespace rdbus;

TEST( TestConfig, InvalidRegisterSpacing )
{
    const auto path = testFilePath + "invalid_register_spacing.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, DuplicateRegisterAddresses )
{
    const auto path = testFilePath + "duplicate_register_addresses.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, DuplicateRegisterNames )
{
    const auto path = testFilePath + "duplicate_register_names.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, DuplicateSlaveIDs )
{
    const auto path = testFilePath + "duplicate_slave_IDs.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, DuplicateSlaveNames )
{
    const auto path = testFilePath + "duplicate_slave_names.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, InvalidProtocol )
{
    const auto path = testFilePath + "invalid_protocol.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        const config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, NoSerial )
{
    const auto path = testFilePath + "no_serial.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, NoSlaves )
{
    const auto path = testFilePath + "no_slaves.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, ValidModbus )
{
    const auto path = testFilePath + "valid_modbus.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Config config = jsonIn;

    EXPECT_EQ( config.protocol, "modbus" );
    EXPECT_FALSE( config.address.has_value() );
    ASSERT_TRUE( config.serial.has_value() );
    EXPECT_EQ( config.serial->baudRate, 4800 );
    EXPECT_EQ( config.modbus.slaves.size(), 1 );
}

TEST( TestConfig, ValidNMEA )
{
    const auto path = testFilePath + "valid_nmea.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Config config = jsonIn;

    EXPECT_EQ( config.protocol, "nmea" );
    EXPECT_FALSE( config.address.has_value() );
    ASSERT_TRUE( config.serial.has_value() );
    EXPECT_EQ( config.serial->baudRate, 4800 );
    EXPECT_EQ( config.nmea.sentences.size(), 2 );
    EXPECT_TRUE( config.nmea.withChecksum );
    EXPECT_EQ( config.nmea.name, "RPM reader" );
}

TEST( TestConfig, NoChecksum )
{
    const auto path = testFilePath + "no_checksum.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, DuplicateSentenceIDs )
{
    const auto path = testFilePath + "duplicate_sentence_IDs.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, NoTalkerID )
{
    const auto path = testFilePath + "no_nmea_name.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, ValidWago )
{
    const auto path = testFilePath + "valid_wago.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Config config = jsonIn;

    EXPECT_EQ( config.protocol, "wago" );
    ASSERT_FALSE( config.serial.has_value() );
    ASSERT_TRUE( config.address.has_value() );
    EXPECT_EQ( config.address->ip, "192.168.10.23" );
    EXPECT_EQ( config.wago.modules.size(), 2 );
    ASSERT_TRUE( config.processors.limits.has_value() );
    EXPECT_EQ( config.processors.limits->devices.size(), 1 );
}

TEST( TestConfig, UnknownLimitDevice )
{
    {
        const auto path = testFilePath + "unknown_limit_device_modbus.json";

        const auto jsonIn = getJsonFromPath( path );

        EXPECT_THROW( {
            config::Config config = jsonIn;
        },
                      config::ParseException );
    }

    {
        const auto path = testFilePath + "unknown_limit_device_wago.json";

        const auto jsonIn = getJsonFromPath( path );

        EXPECT_THROW( {
            config::Config config = jsonIn;
        },
                      config::ParseException );
    }

    {
        const auto path = testFilePath + "unknown_limit_device_nmea.json";

        const auto jsonIn = getJsonFromPath( path );

        EXPECT_THROW( {
            config::Config config = jsonIn;
        },
                      config::ParseException );
    }
}

TEST( TestConfig, NoWagoLimits )
{
    const auto path = testFilePath + "no_wago_limits.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Config config = jsonIn;

    EXPECT_EQ( config.protocol, "wago" );
    ASSERT_FALSE( config.serial.has_value() );
    ASSERT_TRUE( config.address.has_value() );
    EXPECT_EQ( config.wago.modules.size(), 1 );
    EXPECT_FALSE( config.processors.limits.has_value() );
}

TEST( TestConfig, NoWagoModules )
{
    const auto path = testFilePath + "no_wago_modules.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, DuplicateModuleNames )
{
    const auto path = testFilePath + "duplicate_module_names.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, DuplicateInstanceNames )
{
    const auto path = testFilePath + "duplicate_instance_names.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, OffsetGap )
{
    const auto path = testFilePath + "offset_gap.json";

    const auto jsonIn = getJsonFromPath( path );
    const config::Config config = jsonIn;

    auto it = config.wago.modules.begin();
    EXPECT_EQ( it->offset, 1 );
    it++;
    EXPECT_EQ( it->offset, 5 );
    it++;
    EXPECT_EQ( it->offset, 6 );
    it++;
    EXPECT_EQ( it->offset, 8 );
}

TEST( TestConfig, OverlappingOffset )
{
    const auto path = testFilePath + "overlapping_offset.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Config config = jsonIn;
    },
                  config::ParseException );
}

TEST( TestConfig, ConflictingInputTypes )
{
    const auto path = testFilePath + "conflicting_input_types.json";

    const auto jsonIn = getJsonFromPath( path );

    EXPECT_THROW( {
        config::Config config = jsonIn;
    },
                  config::ParseException );
}
