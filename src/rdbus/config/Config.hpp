#pragma once

#include "config/Output.hpp"
#include "config/Serial.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace config
{

struct Config
{
    std::string protocol;
    // std::vector< Slave > slaves;
    Output output;
    Serial serial;
};

} // namespace config
