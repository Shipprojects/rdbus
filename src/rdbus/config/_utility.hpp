#pragma once

#include "exception.hpp"
#include <nlohmann/json.hpp>

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
        throw config::ParseException( exceptionMsg );
    }

    parseKeyValue( j, key, target );
}