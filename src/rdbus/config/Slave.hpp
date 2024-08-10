#pragma once

#include "Register.hpp"
#include <chrono>
#include <list>
#include <nlohmann/json.hpp>
#include <string>

namespace config
{

struct Slave
{
    std::string name;
    int address = 0;

    using Millis = std::chrono::milliseconds;
    Millis pollTimeMs = Millis( 0 );

    using Registers = std::list< Register >;
    Registers registers;

    bool CRC = false;
};

void from_json( const nlohmann::json& j, Slave& x );

} // namespace config
