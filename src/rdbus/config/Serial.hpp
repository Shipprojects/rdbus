#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace config
{

struct Serial
{
    std::string path;
    int baudRate = 0;
    int stopBitsCount = 0;
    bool parity = false;
};

void from_json( const nlohmann::json& j, Serial& x );

} // namespace config
