#pragma once

#include "rdbus/Data.hpp"
#include "rdbus/communication/Connection.hpp"
#include "rdbus/communication/OSWrapper.hpp"
#include "rdbus/config/Serial.hpp"
#include "rdbus/config/nmea/NMEA.hpp"

namespace rdbus::communication::nmea
{

// A high level communication implementation for NMEA
class Communicator
{
public:
    Communicator( const config::Serial& settings, std::unique_ptr< OS > os );

    using NMEA = config::nmea::NMEA;
    // In NMEA we are always reading, listening for talker
    std::optional< rdbus::Data > receive( const NMEA& settings );

private:
    Connection connection;
};

} // namespace rdbus::communication::nmea
