#pragma once

#include <nlohmann/json.hpp>

template < class T >
void getJsonTo( const nlohmann::json& j, const std::string& key, T& target )
{
    if ( j.contains( key ) )
    {
        j.at( key ).get_to( target );
    }
}
