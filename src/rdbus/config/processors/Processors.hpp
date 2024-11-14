#pragma once

#include "Limits.hpp"
#include <nlohmann/json.hpp>
#include <optional>

namespace rdbus::config::processors
{

struct Processors
{
    std::optional< Limits > limits;
};

void from_json( const nlohmann::json& j, Processors& x );

} // namespace rdbus::config::processors
