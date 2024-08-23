#pragma once

#include <chrono>
#include <nlohmann/json.hpp>
#include <string>

namespace rdbus::config
{

struct Serial
{
    std::string path;
    int baudRate = 0;
    int stopBitsCount = 0;
    std::chrono::milliseconds responseTimeout;
    std::chrono::milliseconds lineTimeout;

    enum class Parity
    {
        None,
        Even,
        Odd
    };
    Parity parity = Parity::None;
};

void from_json( const nlohmann::json& j, Serial& x );

} // namespace rdbus::config
