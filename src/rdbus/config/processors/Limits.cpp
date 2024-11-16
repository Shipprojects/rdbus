#include "Limits.hpp"
#include "rdbus/config/utility.hpp"
#include <nlohmann/json.hpp>

using namespace nlohmann;

namespace rdbus::config::processors
{

void from_json( const nlohmann::json& j, Limits& x )
{
    int durationMinutes = 0;
    tools::parseKeyValue( j, "storage_duration_minutes", durationMinutes, "No 'storage_duration_minutes' field present in limits section!" );

    std::list< std::string > devices;
    tools::parseKeyValue( j, "devices", devices, "No 'devices' present in limits section!" );

    tools::throwIf( devices.empty(), "'devices' empty in limits section!" );

    x.devices = devices;
    x.duration = Limits::Minutes( durationMinutes );
}

} // namespace rdbus::config::processors
