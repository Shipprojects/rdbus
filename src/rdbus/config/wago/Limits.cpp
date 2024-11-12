#include "Limits.hpp"
#include "rdbus/config/utility.hpp"
#include <nlohmann/json.hpp>

using namespace nlohmann;

namespace rdbus::config::wago
{

void from_json( const nlohmann::json& j, Limits& x )
{
    int pollTimeMs = 0;
    tools::parseKeyValue( j, "poll_time_ms", pollTimeMs, "No 'poll_time_ms' field present in limits section!" );

    std::list< std::string > modules;
    tools::parseKeyValue( j, "modules", modules, "No 'modules' present in limits section!" );

    tools::throwIf( modules.empty(), "'modules' empty in limits section!" );

    x.modules = modules;
    x.pollTimeMs = Limits::Millis( pollTimeMs );
}

} // namespace rdbus::config::wago
