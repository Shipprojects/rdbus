#include "Field.hpp"
#include "rdbus/config/exception.hpp"
#include "rdbus/config/utility.hpp"

namespace rdbus::config::nmea
{

static Type getStandardType( const std::string& type )
{
    // In NMEA part of rdbus we do not care about precise data types of fields,
    // because we will always know when the field starts and ends using ',' delimiters,
    // so we can safely read data into more general data type fields.
    static std::map< std::string, Type > typeMap = {
        { "F", Type::Double },
        { "U", Type::Uint64 },
        { "S", Type::Int64 },
        { "STR", Type::String }
    };

    const auto& it = typeMap.find( type );
    if ( it == typeMap.end() )
    {
        throw ParseException( "Unknown data type " + type + "!" );
    }

    return it->second;
}

void from_json( const nlohmann::json& j, Field& x )
{
    std::string name;
    tools::parseKeyValue( j, "name", name, "No 'name' field present in 'field' section!" );

    std::string type;
    tools::parseKeyValue( j, "data_type", type, "No 'data_type' field present in 'field' section!" );

    x.name = name;
    x.type = getStandardType( type );
}

} // namespace rdbus::config::nmea