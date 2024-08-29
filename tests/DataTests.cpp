#include "rdbus/Data.hpp"
#include <chrono>
#include <gtest/gtest.h>

using namespace rdbus;
using namespace nlohmann::literals;

TEST( TestData, DataErrorToJson )
{
    Data data;
    data.deviceName = "Sensor";
    data.error = Data::Error{ .code = Data::Error::OS, .what = "Could not read!" };

    const nlohmann::json j = data;

    EXPECT_EQ( j, R"(
        {
            "device": "Sensor",
            "error": {
                "code": 1,
                "what": "Could not read!"
            }
        })"_json );
}

TEST( TestData, DataFieldToJson )
{
    std::tm tm;
    tm.tm_year = 2024 - 1900; // tm_year is year since 1900
    tm.tm_mon = 7;
    tm.tm_mday = 29; // Day of the month
    tm.tm_hour = 16;
    tm.tm_min = 16;
    tm.tm_sec = 54;

    const std::time_t timeSinceEpoch = std::mktime( &tm );
    auto timePoint = std::chrono::system_clock::from_time_t( timeSinceEpoch );
    timePoint += std::chrono::milliseconds( 102 );

    Data::Field field;
    field.name = "Torque";
    field.timestamp = timePoint;
    field.type = Type::Int16;
    field.value = int16_t( 10 );

    const nlohmann::json j = field;

    EXPECT_EQ( j, R"({
                    "name": "Torque",
                    "timestamp": "2024-08-29 16:16:54:102",
                    "value": 10
                })"_json );
}

TEST( TestData, DataToJson )
{
    std::list< Data::Field > fields;

    std::tm tm;
    tm.tm_year = 2024 - 1900; // tm_year is year since 1900
    tm.tm_mon = 7;
    tm.tm_mday = 29; // Day of the month
    tm.tm_hour = 16;
    tm.tm_min = 16;
    tm.tm_sec = 54;

    const std::time_t timeSinceEpoch = std::mktime( &tm );
    auto timePoint = std::chrono::system_clock::from_time_t( timeSinceEpoch );
    timePoint += std::chrono::milliseconds( 102 );

    {
        Data::Field field;
        field.name = "Torque";
        field.timestamp = timePoint;
        field.type = Type::Int16;
        field.value = int16_t( 10 );
        fields.emplace_back( std::move( field ) );
    }

    {
        Data::Field field;
        field.name = "Shaft speed";
        field.timestamp = timePoint;
        field.type = Type::Float;
        field.value = float( -23.959999084472656 );
        fields.emplace_back( std::move( field ) );
    }

    {
        Data::Field field;
        field.name = "Power";
        field.timestamp = timePoint;
        field.type = Type::Uint32;
        field.value = uint32_t( 982 );
        fields.emplace_back( std::move( field ) );
    }

    Data data;
    data.deviceName = "Torque meter";
    data.fields = std::move( fields );

    const nlohmann::json j = data;

    EXPECT_EQ( j, R"(
        {
            "device": "Torque meter",
            "fields": [
                {
                    "name": "Torque",
                    "timestamp": "2024-08-29 16:16:54:102",
                    "value": 10
                },
                {
                    "name": "Shaft speed",
                    "timestamp": "2024-08-29 16:16:54:102",
                    "value": -23.959999084472656
                },
                {
                    "name": "Power",
                    "timestamp": "2024-08-29 16:16:54:102",
                    "value": 982
                }
            ]
        })"_json );
}
