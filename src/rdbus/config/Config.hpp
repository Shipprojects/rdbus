#pragma once

#include "Output.hpp"
#include "Serial.hpp"
#include "Slave.hpp"
#include <list>
#include <nlohmann/json.hpp>
#include <string>

namespace rdbus::config
{

struct Config
{
    std::string protocol;

    using Slaves = std::list< Slave >;
    Slaves slaves;

    Output output;
    Serial serial;
};

void from_json( const nlohmann::json& j, Config& x );

} // namespace rdbus::config
