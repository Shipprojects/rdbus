#include "Adapter.hpp"
#include "MB/modbusException.hpp"

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

Adapter::Adapter( const config::Serial& settings, std::unique_ptr< OS > os )
: connection( settings, std::move( os ) )
{
}

auto Adapter::send( const Request& request ) -> Response
{
    connection.sendData( toRaw( request ) );

    const auto& rawed = connection.getData();

    try
    {
        return Response::fromRawCRC( rawed );
    }
    // Original modbus exception in ModbusResponse is not created from raw data
    // and thus contains less information. Create new exception from raw data here.
    catch ( const MB::ModbusException& )
    {
        throw MB::ModbusException( rawed, true );
    }
}

} // namespace rdbus::communication::modbus
