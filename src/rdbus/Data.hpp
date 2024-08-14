#pragma once

#include <chrono>
#include <list>
#include <nlohmann/json.hpp>
#include <string>
#include <variant>

namespace rdbus
{

enum class Type
{
    Float,
    Double,
    Uint16,
    Int16,
    Uint32,
    Int32,
    Blob
};

struct Data
{
    std::string deviceName;

    struct Field
    {
        std::string name;
        std::variant< float, double, uint32_t, int32_t, uint16_t, int16_t > value;

        Type type;

        using Timestamp = std::chrono::time_point< std::chrono::system_clock >;
        Timestamp timestamp;
    };

    std::list< Field > fields;
};

void to_json( nlohmann::json& j, const Data& x );
void to_json( nlohmann::json& j, const Data::Field& x );

} // namespace rdbus
