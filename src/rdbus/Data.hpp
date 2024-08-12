#pragma once

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

struct Output
{
    std::string deviceName;

    struct Field
    {
        std::string name;
        std::variant< float, double, uint32_t, int32_t > value;

        Type type;
    };

    std::list< Field > fields;
};


} // namespace rdbus
