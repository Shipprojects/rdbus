#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace rdbus::config
{

struct Output
{
    using IP = std::string;
    using Port = int;
    std::optional< Port > port;
    std::optional< IP > ip;

    enum Type
    {
        Stdout,
        TCP_IP
    };
    Type type;
};


void from_json( const nlohmann::json& j, Output& x );

} // namespace rdbus::config
