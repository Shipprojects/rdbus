#include "Module.hpp"
#include "rdbus/config/utility.hpp"
#include <nlohmann/json.hpp>

using namespace nlohmann;

namespace rdbus::config::ip
{

void from_json( const nlohmann::json& j, Module& x )
{
    std::string name;
    tools::parseKeyValue( j, "name", name, "No 'name' field present in module!" );

    int pollTimeMs = 0;
    tools::parseKeyValue( j, "poll_time_ms", pollTimeMs, "No 'poll_time_ms' field present in module!" );

    std::list< std::string > instances;
    tools::parseKeyValue( j, "instances", instances, "No 'instances' present in module!" );

    int absoluteOffset = 0;
    tools::parseKeyValue( j, "instances_absolute_offset", absoluteOffset );

    tools::throwIf( absoluteOffset < 0, "'instances_absolute_offset' cannot be less than 0!" );
    tools::throwIf( instances.empty(), "'instances' empty in module!" );
    tools::throwIf( instances.size() != 1 &&
                    instances.size() != 2 &&
                    instances.size() != 4 &&
                    instances.size() != 8,
                    "Count of instances must be 1, 2, 4 or 8!" );

    x.name = name;
    x.absoluteOffset = absoluteOffset;
    x.instances = instances;
    x.pollTimeMs = Module::Millis( pollTimeMs );
}

} // namespace rdbus::config::ip