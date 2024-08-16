#include "Adapter.hpp"

namespace rdbus::communication::modbus
{

static std::vector< uint8_t > toRaw( const MB::ModbusRequest& request )
{
    auto rawed = request.toRaw();

    // Add 2 CRC bytes at the end of the request
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

auto Adapter::send( const Request& request, std::chrono::seconds requestTimeout ) -> Response
{
    connection.sendData( toRaw( request ) );

    const auto& rawed = connection.getData( requestTimeout );

    return Response::fromRawCRC( rawed );
}

} // namespace rdbus::communication::modbus
