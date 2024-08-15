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

static std::string formattedTime( Data::Field::Timestamp timestamp )
{
    const auto millis = timestamp.time_since_epoch().count() / 1000000 % 1000;

    const auto t = std::chrono::system_clock::to_time_t( timestamp );
    const auto time = std::localtime( &t );
    char buf[ 32 ] = {};
    // Formats time string to 'YYYY-MM-DD hh:mm:ss'
    std::strftime( buf, sizeof( buf ), "%F %T", time );
    // Adds milliseconds in the end
    sprintf( buf, "%s:%03d", buf, static_cast< int >( millis ) );

    return std::string( buf );
}

void to_json( nlohmann::json& j, const Data::Field& x )
{
    j[ "name" ] = x.name;
    j[ "timestamp" ] = formattedTime( x.timestamp );

    switch ( x.type )
    {
        case Type::Int16:
            j[ "value" ] = std::get< int16_t >( x.value );
            break;
        case Type::Uint16:
            j[ "value" ] = std::get< uint16_t >( x.value );
            break;
        case Type::Int32:
            j[ "value" ] = std::get< int32_t >( x.value );
            break;
        case Type::Uint32:
            j[ "value" ] = std::get< uint32_t >( x.value );
            break;
        case Type::Int64:
            j[ "value" ] = std::get< int64_t >( x.value );
            break;
        case Type::Uint64:
            j[ "value" ] = std::get< uint64_t >( x.value );
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