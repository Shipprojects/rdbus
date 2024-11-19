#include "Instance.hpp"
#include "rdbus/config/utility.hpp"

namespace rdbus::config::wago
{

void from_json( const nlohmann::json& j, Instance& x )
{
    std::string name;
    tools::parseKeyValue( j, "name", name, "No 'name' value set in instance!" );

    int16_t min;
    tools::parseKeyValue( j, "min", min, "No 'min' value set in instance!" );

    int16_t max;
    tools::parseKeyValue( j, "max", max, "No 'max' value set in instance!" );

    tools::throwIf( min > max, "Invalid instance range, 'min' cannot be bigger than 'max!'" );

    x.min = min;
    x.max = max;
    x.name = name;
}

} // namespace rdbus::config::wago
