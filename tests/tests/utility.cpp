#include "utility.hpp"
#include <fstream>
#include <nlohmann/json.hpp>

nlohmann::json getJsonFromPath( const std::string& path )
{
    std::ifstream jsonFile( path );
    if ( !jsonFile.good() )
    {
        return nlohmann::json();
    }

    return nlohmann::json::parse( jsonFile );
}
