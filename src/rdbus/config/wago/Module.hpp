#pragma once

#include "Instance.hpp"
#include <chrono>
#include <list>
#include <nlohmann/json.hpp>
#include <string>

namespace rdbus::config::wago
{

struct Module
{
    std::string name;

    using Millis = std::chrono::milliseconds;
    Millis pollTimeMs = Millis( 0 );

    std::list< Instance > instances;

    // Absolute instance offset from all fields of other instances
    // Instances start at 1
    constexpr static unsigned int defaultOffset = 1;
    unsigned int offset = defaultOffset;
};

void from_json( const nlohmann::json& j, Module& x );

} // namespace rdbus::config::wago
