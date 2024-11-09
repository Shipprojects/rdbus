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

    // A list of modules for which to store and process min and max values of their fields
    std::list< std::string > modules;
};

void from_json( const nlohmann::json& j, Limits& x );

} // namespace rdbus::config::ip
