#include "Config.hpp"
#include "_utility.hpp"
#include "exception.hpp"
#include <nlohmann/json.hpp>

using namespace nlohmann;

namespace rdbus::config
{

void from_json( const nlohmann::json& j, Config& x )
{
    std::string protocol;
    parseKeyValue( j, "protocol", protocol, "No protocol name present!" );

    Output output;
    parseKeyValue( j, "output", output, "No output section present!" );

    Serial serial;
    parseKeyValue( j, "serial", serial, "No serial section present!" );

    Config::Slaves slaves;
    parseKeyValue( j, "slaves", slaves, "No slaves present!" );

    if ( slaves.empty() )
    {
        throw ParseException( "No slaves present!" );
    }

    const std::list< std::string > availableProtocols = { "modbus", "nmea" };
    if ( std::find( availableProtocols.begin(), availableProtocols.end(), protocol ) == availableProtocols.end() )
    {
        throw ParseException( "Unsupported protocol selected!" );
    }

    x.output = output;
    x.protocol = protocol;
    x.serial = serial;
    x.slaves = slaves;
}

void validate( const Config& )
{
}

} // namespace rdbus::config