#include "Processors.hpp"
#include "Limits.hpp"
#include "rdbus/config/utility.hpp"

namespace rdbus::config::processors
{

void from_json( const nlohmann::json& j, Processors& x )
{
    Limits limits;
    tools::parseKeyValue( j, "limits", limits );

    x.limits = limits;
}

} // namespace rdbus::config::processors