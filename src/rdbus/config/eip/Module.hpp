#pragma once

#include <chrono>
#include <list>
#include <nlohmann/json.hpp>
#include <string>

namespace rdbus::config::eip
{

struct Module
{
    std::string name;

    using Millis = std::chrono::milliseconds;
    Millis pollTimeMs = Millis( 0 );

    std::list< std::string > instances;

    // Absolute instance offset from all fields of other instances
    // Instances start at 1
    constexpr static unsigned int defaultOffset = 1;
    unsigned int offset = defaultOffset;
};

void from_json( const nlohmann::json& j, Module& x );

} // namespace rdbus::config::eip
