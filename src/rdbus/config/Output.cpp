#include "Output.hpp"
#include "utility.hpp"

using namespace nlohmann;

namespace rdbus::config
{

void from_json( const json& j, Output& x )
{
    constexpr int invalidPort = -1;
    int port = invalidPort;
    tools::parseKeyValue( j, "port", port );

    std::string ip;
    tools::parseKeyValue( j, "ip", ip );

    std::string type;
    tools::parseKeyValue( j, "type", type, "Missing 'type' field in 'output' section!" );

    tools::throwIf( type != "stdout" && type != "TCP/IP", "The only allowed output types are 'stdout' and 'TCP/IP'!" );
    tools::throwIf( type == "stdout" && ( !ip.empty() || port != invalidPort ), "Multiple output types detected! Do not mix 'ip/port' with stdout 'type'!" );
    tools::throwIf( type == "TCP/IP" && port == invalidPort, "No 'port' field!" );
    tools::throwIf( type == "TCP/IP" && ip.empty(), "No 'ip' field!" );

    if ( type == "stdout" )
    {
        x.type = Output::Stdout;
    }
    else if ( type == "TCP/IP" )
    {
        x.ip = ip;
        x.port = port;
        x.type = Output::TCP_IP;
    }
}

} // namespace rdbus::config
