#pragma once

#include <nlohmann/json.hpp>
#include <fstream>

nlohmann::json getJsonFromPath( const std::string& path )
{
    std::ifstream jsonFile( path );
    if ( !jsonFile.good() )
    {
        return nlohmann::json();
    }

    return nlohmann::json::parse( jsonFile );
}
