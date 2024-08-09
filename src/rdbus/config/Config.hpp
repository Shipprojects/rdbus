#pragma once

#include "config/Output.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace config
{

struct Config
{
    Config( const std::string& protocol /*const std::vector<Slave> slaves, const Output& output*/ );

    std::string protocol;
    // std::vector< Slave > slaves;
    Output output;
};

} // namespace config
