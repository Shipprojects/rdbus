#include "Serial.hpp"
#include "_utility.hpp"
#include "exception.hpp"

using namespace nlohmann;

namespace config
{

void from_json( const nlohmann::json& j, Serial& x )
{
    std::string path;
    parseKeyValue( j, "path", path, "No serial device path field present in 'serial' section!" );

    int baudRate = 0;
    parseKeyValue( j, "baud_rate", baudRate, "No baud rate field present in 'serial' section!" );

    int stopBitsCount = 0;
    parseKeyValue( j, "stop_bits_count", stopBitsCount, "No stop bits field present in 'serial' section!" );

    int parity = false;
    parseKeyValue( j, "parity", parity, "No parity field present in 'serial' section!" );

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

} // namespace config