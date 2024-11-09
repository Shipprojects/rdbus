#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace rdbus::config
{

struct Address
{
    std::string ip;
    int port;
};

void from_json( const nlohmann::json& j, Address& x );

} // namespace rdbus::config
