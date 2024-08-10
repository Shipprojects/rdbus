#include "Adapter.hpp"

namespace communication::modbus
{

static std::vector< uint8_t > toRaw( const MB::ModbusRequest& request )
{
    auto rawed = request.toRaw();

    const uint16_t CRC = MB::utils::calculateCRC( rawed );

    const uint8_t firstByte = reinterpret_cast< const uint8_t* >( &CRC )[ 0 ];
    const uint8_t secondByte = reinterpret_cast< const uint8_t* >( &CRC )[ 1 ];
    rawed.push_back( firstByte );
    rawed.push_back( secondByte );

    return rawed;
}

Adapter::Adapter( const config::Serial& settings )
: connection( settings )
{
}

auto Adapter::request( const Request& request ) -> Response
{
    connection.sendData( toRaw( request ) );

    // Hardcoded request timeout
    constexpr std::chrono::seconds requestTimeout( 15 );
    return Response( connection.getData( std::chrono::seconds( requestTimeout ) ), true );
}

} // namespace communication::modbus
