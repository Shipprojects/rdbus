#pragma once

#include "Address.hpp"
#include <nlohmann/json.hpp>
#include <optional>

namespace rdbus::config
{

struct Output
{
    std::optional< Address > address;

    enum Type
    {
        Stdout,
        TCP_IP
    };
    Type type;
};


void from_json( const nlohmann::json& j, Output& x );

} // namespace rdbus::config
