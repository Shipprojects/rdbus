#pragma once

#include "Exception.hpp"
#include <nlohmann/json.hpp>

namespace rdbus::config::tools
{

template < class T >
bool parseKeyValue( const nlohmann::json& j, const std::string& key, T& target )
{
    if ( j.contains( key ) )
    {
        j.at( key ).get_to( target );
        return true;
    }

    return false;
}

template < class T >
void parseKeyValue( const nlohmann::json& j, const std::string& key, T& target, const std::string& exceptionMsg )
{
    if ( !j.contains( key ) )
    {
        throw rdbus::config::ParseException( exceptionMsg );
    }

    parseKeyValue( j, key, target );
}

void throwIf( bool condition, const std::string& message );

} // namespace rdbus::config::tools