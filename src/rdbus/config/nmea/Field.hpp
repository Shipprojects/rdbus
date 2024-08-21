#pragma once

#include <nlohmann/json.hpp>
#include "rdbus/Data.hpp"

namespace rdbus::config::nmea
{

struct Field
{
    std::string name;
    Type type;
};

void from_json( const nlohmann::json& j, Field& x );

} // namespace rdbus::config::nmea
