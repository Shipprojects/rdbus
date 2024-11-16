#include "Data.hpp"
#include "config/Exception.hpp"
#include <chrono>
#include <nlohmann/json.hpp>

namespace rdbus
{

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

void to_json( nlohmann::json& j, const Data::Field::Variant& x )
{
    if ( std::holds_alternative< int16_t >( x ) )
    {
        j = std::get< int16_t >( x );
    }
    else if ( std::holds_alternative< uint16_t >( x ) )
    {
        j = std::get< uint16_t >( x );
    }
    else if ( std::holds_alternative< int32_t >( x ) )
    {
        j = std::get< int32_t >( x );
    }
    else if ( std::holds_alternative< uint32_t >( x ) )
    {
        j = std::get< uint32_t >( x );
    }
    else if ( std::holds_alternative< int64_t >( x ) )
    {
        j = std::get< int64_t >( x );
    }
    else if ( std::holds_alternative< uint64_t >( x ) )
    {
        j = std::get< uint64_t >( x );
    }
    else if ( std::holds_alternative< float >( x ) )
    {
        j = std::get< float >( x );
    }
    else if ( std::holds_alternative< double >( x ) )
    {
        j = std::get< double >( x );
    }
    else if ( std::holds_alternative< std::string >( x ) )
    {
        j = std::get< std::string >( x );
    }
    else
    {
        throw config::ParseException( "Unknown variant type!" );
    }
}

void to_json( nlohmann::json& j, const Data::Field& x )
{
    j[ "name" ] = x.name;
    j[ "timestamp" ] = formattedTime( x.timestamp );

    if ( x.value.has_value() )
    {
        to_json( j[ "value" ], x.value.value() );
    }
    else
    {
        j[ "value" ] = nullptr;
    }
}

void to_json( nlohmann::json& j, const Data::Error& x )
{
    j[ "code" ] = static_cast< int >( x.code );
    j[ "what" ] = x.what;
}

void to_json( nlohmann::json& j, const Data& x )
{
    j[ "device" ] = x.deviceName;

    if ( x.error.has_value() )
    {
        j[ "error" ] = x.error.value();
    }
    else
    {
        j[ "fields" ] = x.fields;
    }

    if ( x.metadata.has_value() )
    {
        j[ "metadata" ] = x.metadata.value();
    }
}

} // namespace rdbus