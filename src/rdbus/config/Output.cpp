#include "Output.hpp"
#include "_utility.hpp"
#include "exception.hpp"

using namespace nlohmann;

namespace config
{

void from_json( const json& j, Output& x )
{
    int port = -1;
    getJsonTo( j, "port", port );

    std::string ip;
    getJsonTo( j, "ip", ip );

    std::string path;
    getJsonTo( j, "path", path );

    if ( !path.empty() && ( !ip.empty() || port != -1 ) )
    {
        throw ParseException( "Multiple output types detected! Use 'file' or 'ip/port' fields exclusively!" );
    }

    if ( !path.empty() )
    {
        x.filePath = path;
        x.type = Output::File;
    }

    if ( !ip.empty() )
    {
        x.ipPortPair->first = ip;
        x.ipPortPair->second = port;
        x.type = Output::TCP_IP;
    }
}

} // namespace config
