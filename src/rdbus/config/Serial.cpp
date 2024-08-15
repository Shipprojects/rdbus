#include "Serial.hpp"
#include "exception.hpp"
#include "utility.hpp"

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

    int parity = false;
    tools::parseKeyValue( j, "parity", parity, "No parity field present in 'serial' section!" );

    if ( !path.starts_with( '/' ) )
    {
        throw ParseException( "Absolute path required for serial device!" );
    }

    if ( stopBitsCount > 2 )
    {
        throw ParseException( "There cannot be more than 2 stop bits!" );
    }

    x.baudRate = baudRate;
    x.path = path;
    x.parity = parity;
    x.stopBitsCount = stopBitsCount;
}

} // namespace rdbus::config