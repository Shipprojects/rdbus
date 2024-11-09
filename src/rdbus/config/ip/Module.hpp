#pragma once

#include <chrono>
#include <list>
#include <nlohmann/json.hpp>
#include <string>

namespace rdbus::config::ip
{

struct Module
{
    std::string name;

    using Millis = std::chrono::milliseconds;
    Millis pollTimeMs = Millis( 0 );

    std::list< std::string > instances;

    unsigned int absoluteOffset = 0;
};

void from_json( const nlohmann::json& j, Module& x );

} // namespace rdbus::config::ip
