#pragma once

#include <nlohmann/json.hpp>

namespace rdbus::config::wago
{

struct Instance
{
    std::string name;

    int16_t min = 0;
    int16_t max = 0;
};

void from_json( const nlohmann::json& j, Instance& x );

} // namespace rdbus::config
