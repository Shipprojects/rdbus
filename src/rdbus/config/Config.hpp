#pragma once

#include "Address.hpp"
#include "Serial.hpp"
#include "ip/IP.hpp"
#include "modbus/Modbus.hpp"
#include "nmea/NMEA.hpp"
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
    ip::IP ip; // Industrial Protocol, not to be confused with Internet Protocol!

    // Only one of these two is available for any given configuration
    std::optional< Serial > serial;
    std::optional< Address > address;
};

void from_json( const nlohmann::json& j, Config& x );

} // namespace rdbus::config
