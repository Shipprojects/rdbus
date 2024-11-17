#pragma once

#include "Address.hpp"
#include "Serial.hpp"
#include "processors/Processors.hpp"
#include "modbus/Modbus.hpp"
#include "nmea/NMEA.hpp"
#include "wago/Wago.hpp"
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace rdbus::config
{

struct Config
{
    std::string protocol;

    modbus::Modbus modbus;
    nmea::NMEA nmea;
    wago::Wago wago;
    processors::Processors processors;

    // Only one of these two is available for any given configuration
    std::optional< Serial > serial;
    std::optional< Address > address;
};

void from_json( const nlohmann::json& j, Config& x );

} // namespace rdbus::config
