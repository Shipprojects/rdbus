#pragma once

#include "Register.hpp"
#include <chrono>
#include <list>
#include <nlohmann/json.hpp>
#include <string>

namespace rdbus::config
{

struct Slave
{
    std::string name;
    int id = 0;

    using Millis = std::chrono::milliseconds;
    Millis pollTimeMs = Millis( 0 );

    using Registers = std::list< Register >;
    Registers inputRegisters;
    Registers outputRegisters;
};

void from_json( const nlohmann::json& j, Slave& x );

} // namespace rdbus::config
