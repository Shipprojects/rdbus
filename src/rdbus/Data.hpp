#pragma once

#include <chrono>
#include <list>
#include <string>
#include <variant>

namespace rdbus
{

enum class Type
{
    Float,
    Double,
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
        std::variant< float, double, uint32_t, int32_t > value;

        Type type;

        using Timestamp = std::chrono::time_point< std::chrono::system_clock >;
        Timestamp timestamp;
    };

    std::list< Field > fields;
};


} // namespace rdbus
