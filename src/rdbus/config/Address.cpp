#include "Address.hpp"
#include "utility.hpp"
#include <nlohmann/json.hpp>

using namespace nlohmann;

namespace rdbus::config
{

void from_json( const nlohmann::json& j, Address& x )
{
    int port = 0;
    tools::parseKeyValue( j, "port", port, "No 'port' address present!" );

    std::string ip;
    tools::parseKeyValue( j, "ip", ip, "No 'ip' address present!" );

    tools::throwIf( ip.empty(), "No 'ip' field!" );

    x.ip = ip;
    x.port = port;
}

} // namespace rdbus::config
