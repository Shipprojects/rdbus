#include "RequestPlanner.hpp"
#include "MB/modbusRequest.hpp"
#include "MB/modbusUtils.hpp"
#include "rdbus/communication/modbus/RequestDescription.hpp"
#include "rdbus/config/Register.hpp"
#include <numeric>

namespace rdbus::communication::modbus
{

using namespace config;
RequestDescriptions requestPlan( const Slave& slave )
{
    RequestDescriptions requestDescriptions;

    // Sort registers for easier address grouping
    auto registers = slave.registers;
    registers.sort( []( const Register& left, const Register& right )
                    { return left.address < right.address; } );


    // Starting address of register group
    int address = registers.begin()->address;
    std::list< Register > group;
    for ( auto it = registers.begin(); it != registers.end(); it++ )
    {
        group.push_back( *it );

        // If next register is the last register or next
        // register is not adjacent to the current interpreted one
        if ( ( std::next( it ) == registers.end() ) ||
             ( it->address + ( it->byteOrder.size() / sizeof( uint16_t ) ) != std::next( it )->address ) )
        {
            const int rawRegisterCount = std::accumulate( group.begin(), group.end(), 0,
                                                          []( int accumulator, const Register& next )
                                                          { return next.byteOrder.size() / sizeof( uint16_t ) + accumulator; } );

            // TODO: Will probably need to find a way how to read from coils too, otherwise the only reading
            // that is possible now is from holding registers due to hardcoded function code
            auto request = MB::ModbusRequest( slave.address,
                                              MB::utils::MBFunctionCode::ReadAnalogOutputHoldingRegisters,
                                              address, rawRegisterCount );
            requestDescriptions.push_back( { request, group } );
            group.clear();

            if ( std::next( it ) != registers.end() )
            {
                // Store the new group starting address
                address = std::next( it )->address;
            }
        }
    }

    return requestDescriptions;
}

} // namespace rdbus::communication::modbus
