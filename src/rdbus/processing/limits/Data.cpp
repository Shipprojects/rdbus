#include "Data.hpp"
#include "rdbus/Data.hpp"

namespace rdbus::processing::limits
{

static void to_json( nlohmann::json& j, const Data::FieldLimit& x )
{
    j[ "name" ] = x.name;

    if ( x.min.has_value() )
    {
        rdbus::to_json( j[ "min" ], x.min.value() );
    }
    else
    {
        j[ "min" ] = nullptr;
    }

    if ( x.max.has_value() )
    {
        rdbus::to_json( j[ "max" ], x.max.value() );
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

const std::string& Data::getDeviceName() const
{
    return deviceName;
}

nlohmann::json Data::toJson() const
{
    return *this;
}

} // namespace rdbus::processing::limits
