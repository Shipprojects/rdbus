#pragma once

#include "Serial.hpp"
#include "modbus/Modbus.hpp"
#include "nmea/NMEA.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace rdbus::config
{

struct Config
{
    std::string protocol;

    modbus::Modbus modbus;
    nmea::NMEA nmea;

    Serial serial;
};

void from_json( const nlohmann::json& j, Config& x );

} // namespace rdbus::config
