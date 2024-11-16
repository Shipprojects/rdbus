#pragma once

#include <chrono>
#include <list>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <variant>

namespace rdbus
{

enum class Type
{
    None,
    String,
    Float,
    Double,
    Uint16,
    Int16,
    Uint32,
    Int32,
    Uint64,
    Int64
};

// All data from single device
struct Data
{
    std::string deviceName;

    struct Field
    {
        std::string name;

        using Variant =
        std::variant< std::string,
                      float,
                      double,
                      uint64_t,
                      int64_t,
                      uint32_t,
                      int32_t,
                      uint16_t,
                      int16_t >;

        Variant value;

        Type type;

        using Timestamp = std::chrono::time_point< std::chrono::system_clock >;
        Timestamp timestamp;
    };

    std::list< Field > fields;

    struct Error
    {
        enum Code
        {
            OS = 1,
            Modbus,
            NMEA,
            Wago
        };

        Code code;
        std::string what;
    };
    std::optional< Error > error;

    std::optional< std::string > metadata;
};

void to_json( nlohmann::json& j, const Data& x );
void to_json( nlohmann::json& j, const Data::Field::Variant& x );
void to_json( nlohmann::json& j, const Data::Error& x );
void to_json( nlohmann::json& j, const Data::Field& x );

} // namespace rdbus
