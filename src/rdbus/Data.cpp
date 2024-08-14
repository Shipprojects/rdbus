#include "Data.hpp"
#include <chrono>
#include <nlohmann/json.hpp>

namespace rdbus
{

struct ParseException : public std::runtime_error
{
    ParseException( const std::string& what )
    : std::runtime_error( what )
    {
    }
};

void to_json( nlohmann::json& j, const Data::Field& x )
{
    j[ "name" ] = x.name;

    const auto time = std::chrono::system_clock::to_time_t( x.timestamp );
    j[ "timestamp" ] = std::ctime( &time );

    switch ( x.type )
    {
        case Type::Int32:
            j[ "value" ] = std::get< int32_t >( x.value );
            break;
        case Type::Uint32:
            j[ "value" ] = std::get< uint32_t >( x.value );
            break;
        case Type::Float:
            j[ "value" ] = std::get< float >( x.value );
            break;
        case Type::Double:
            j[ "value" ] = std::get< double >( x.value );
            break;
        default:
            throw ParseException( "Unknown argument!" );
            break;
    }
}

void to_json( nlohmann::json& j, const Data& x )
{
    j[ "device" ] = x.deviceName;
    j[ "registers" ] = x.fields;
}

} // namespace rdbus