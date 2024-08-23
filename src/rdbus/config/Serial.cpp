#include "Serial.hpp"
#include "exception.hpp"
#include "utility.hpp"
#include <chrono>

using namespace nlohmann;

namespace rdbus::config
{

void from_json( const nlohmann::json& j, Serial& x )
{
    std::string path;
    tools::parseKeyValue( j, "path", path, "No serial device path field present in 'serial' section!" );

    int baudRate = 0;
    tools::parseKeyValue( j, "baud_rate", baudRate, "No baud rate field present in 'serial' section!" );

    int stopBitsCount = 0;
    tools::parseKeyValue( j, "stop_bits_count", stopBitsCount, "No stop bits field present in 'serial' section!" );

    std::string parityStr;
    tools::parseKeyValue( j, "parity", parityStr, "No parity field present in 'serial' section!" );

    int responseTimeout = 0;
    tools::parseKeyValue( j, "response_timeout_ms", responseTimeout, "No response_timeout_ms field present in 'serial' section!" );

    int lineTimeout = 0;
    tools::parseKeyValue( j, "line_timeout_ms", lineTimeout, "No line_timeout_ms field present in 'serial' section!" );

    tools::throwIf( !path.starts_with( '/' ), "Absolute path required for serial device!" );
    tools::throwIf( stopBitsCount > 2, "There cannot be more than 2 stop bits!" );
    tools::throwIf( stopBitsCount < 1, "There cannot be less than 1 stop bit!" );

    Serial::Parity parity;
    if ( parityStr == "none" )
    {
        parity = Serial::Parity::None;
    }
    else if ( parityStr == "even" )
    {
        parity = Serial::Parity::Even;
    }
    else if ( parityStr == "odd" )
    {
        parity = Serial::Parity::Odd;
    }
    else
    {
        throw ParseException( "Unknown parity '" + parityStr + "'!" );
    }

    x.baudRate = baudRate;
    x.path = path;
    x.parity = parity;
    x.stopBitsCount = stopBitsCount;
    x.responseTimeout = std::chrono::milliseconds( responseTimeout );
    x.lineTimeout = std::chrono::milliseconds( lineTimeout );
}

} // namespace rdbus::config