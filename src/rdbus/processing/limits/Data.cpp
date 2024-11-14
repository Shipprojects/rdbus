#include "Data.hpp"

namespace rdbus::processing::limits
{

static void to_json( nlohmann::json& j, const Data::FieldLimit& x )
{
    j[ "name" ] = x.name;

    if ( x.min.has_value() )
    {
        j[ "min" ] = x.min.value();
    }
    else
    {
        j[ "min" ] = nullptr;
    }

    if ( x.max.has_value() )
    {
        j[ "max" ] = x.max.value();
    }
    else
    {
        j[ "max" ] = nullptr;
    }
}

void to_json( nlohmann::json& j, const Data& x )
{
    j[ "device" ] = x.deviceName;
    j[ "fields" ] = x.instanceLimits;
}

} // namespace rdbus::processing::limits
