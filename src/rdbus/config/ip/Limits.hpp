#pragma once

#include <chrono>
#include <list>
#include <nlohmann/json.hpp>
#include <string>

namespace rdbus::config::ip
{

struct Limits
{
    using Millis = std::chrono::milliseconds;
    Millis pollTimeMs = Millis( 0 );

    std::list< std::string > modules;
};

void from_json( const nlohmann::json& j, Limits& x );

} // namespace rdbus::config::ip
