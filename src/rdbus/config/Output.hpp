#pragma once

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <utility>

namespace rdbus::config
{

struct Output
{
    using FilePath = std::optional< std::string >;
    FilePath filePath;

    using IP = std::string;
    using Port = int;
    std::optional< std::pair< IP, Port > > ipPortPair;

    enum Type
    {
        File,
        TCP_IP
    };
    Type type;
};


void from_json( const nlohmann::json& j, Output& x );

} // namespace rdbus::config
