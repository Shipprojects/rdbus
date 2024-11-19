#pragma once

#include "utility.hpp"
#include <nlohmann/json.hpp>

namespace rdbus::config
{

template < class T >
struct Range
{
    T min = 0;
    T max = 0;
};

template < class T >
void from_json( const nlohmann::json& j, Range< T >& x )
{
    T min;
    tools::parseKeyValue( j, "min", min, "No 'min' value set in range!" );

    T max;
    tools::parseKeyValue( j, "max", max, "No 'max' value set in range!" );

    tools::throwIf( min > max, "Invalid range, 'min' cannot be bigger than 'max!'" );

    x.min = min;
    x.max = max;
}

} // namespace rdbus::config
