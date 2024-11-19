#include "Communicator.hpp"
#include "rdbus/Data.hpp"
#include "rdbus/communication/EIP.hpp"
#include "rdbus/config/Address.hpp"
#include "rdbus/config/wago/Module.hpp"
#include <cip/EPath.h>
#include <spdlog/spdlog.h>

using namespace rdbus::config::wago;
using namespace rdbus::config;

namespace rdbus::communication::wago
{

Communicator::Communicator( const Address& address )
: address( address )
{
}

// Wago encodes analog data of 12-bit resolution (1 bit is given to the sign)
// into a 16-bit word, where the first 4 least significant bits contain
// diagnostic information.
// The order of the 16-bit word is little endian.
static int16_t extractAnalog( int16_t value )
{
    const int16_t sign = value & 0x8000; // Extract the sign of the value
    value &= ~( 0x8000 ); // Remove the sign in the value, so we could safely shift it without the sign being interpreted as a numerical part
    value >>= 4; // Remove the diagnostic part (the first 4 bits)
    value |= sign; // Add back the sign
    return value;
}

static int16_t map( int16_t x, int16_t in_min, int16_t in_max, int16_t out_min, int16_t out_max )
{
    return ( x - in_min ) * ( out_max - out_min ) / ( in_max - in_min ) + out_min;
}

rdbus::Data Communicator::request( const Module& module )
{
    rdbus::Data data;
    data.deviceName = module.name;

    try
    {
        int offset = module.offset;
        for ( const auto& instance : module.instances )
        {
            // 0x67 - Wago Analog Input Point
            // offset - instance ID
            // 1 - analog input value attribute ID
            const eipScanner::cip::EPath epath( 0x67, offset++, 1 );
            constexpr uint8_t serviceId = 0x0e; /*Get_Attribute_Single service for Wago classes 0x65 and 0x67*/
            EIP eip( address );
            const int16_t raw = eip.getSingle( epath, serviceId );

            constexpr int16_t minAnalogValue = 0;
            constexpr int16_t maxAnalogValue = 0x7ff; // Max value of 11 bit integer
            const auto value = map( extractAnalog( raw ), minAnalogValue, maxAnalogValue, instance.min, instance.max );

            data.fields.emplace_back( rdbus::Data::Field{ .name = instance.name,
                                                          .value = value,
                                                          .timestamp = std::chrono::system_clock::now() } );
        }
    }
    catch ( const EIP::Exception& e )
    {
        SPDLOG_ERROR( e.what() );
        data.fields.clear();
        data.error = rdbus::Data::Error{ .code = rdbus::Data::Error::OS,
                                         .what = e.what() };
    }
    catch ( const std::exception& e )
    {
        SPDLOG_ERROR( e.what() );
        data.fields.clear();
        data.error = rdbus::Data::Error{ .code = rdbus::Data::Error::OS,
                                         .what = e.what() };
    }

    return data;
}

} // namespace rdbus::communication::wago