#include "Output.hpp"
#include "Address.hpp"
#include "utility.hpp"

using namespace nlohmann;

namespace rdbus::config
{

void from_json( const json& j, Output& x )
{
    std::string type;
    tools::parseKeyValue( j, "type", type, "Missing 'type' field in 'output' section!" );

    tools::throwIf( type != "stdout" && type != "TCP/IP", "The only allowed output types are 'stdout' and 'TCP/IP'!" );
    tools::throwIf( type == "stdout" && j.contains( "address" ), "Multiple output types detected! Do not mix 'address' with stdout 'type'!" );

    if ( type == "stdout" )
    {
        x.type = Output::Stdout;
    }
    else if ( type == "TCP/IP" )
    {
        Address address;
        tools::parseKeyValue( j, "address", address, "Missing 'address' section for 'TCP/IP' output type!" );
        x.address = address;
        x.type = Output::TCP_IP;
    }
}

} // namespace rdbus::config
