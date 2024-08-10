#include "MessageBuilder.hpp"
#include "MB/modbusUtils.hpp"
#include <cstdint>

namespace communication
{

namespace modbus
{

std::vector< uint8_t > toRawRequest( const MB::ModbusRequest& request, const config::Slave& settings )
{
    auto rawed = request.toRaw();

    if ( settings.CRC )
    {
        const uint16_t CRC = MB::utils::calculateCRC( rawed );

        const uint8_t firstByte = reinterpret_cast< const uint8_t* >( &CRC )[ 0 ];
        const uint8_t secondByte = reinterpret_cast< const uint8_t* >( &CRC )[ 1 ];
        rawed.push_back( firstByte );
        rawed.push_back( secondByte );
    }

    return rawed;
}

} // namespace modbus

} // namespace communication