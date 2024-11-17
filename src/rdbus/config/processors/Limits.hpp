#pragma once

#include <chrono>
#include <list>
#include <nlohmann/json.hpp>
#include <string>

namespace rdbus::config::processors
{

struct Limits
{
    using Minutes = std::chrono::minutes;
    // A time duration on how long to keep a record for
    Minutes duration = Minutes( 0 );

    // A list of devices for which to store and process min and max values of their fields
    std::list< std::string > devices;
};

void from_json( const nlohmann::json& j, Limits& x );

} // namespace rdbus::config::processors
