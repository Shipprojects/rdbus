#include "RequestPlanner.hpp"
#include "MB/modbusRequest.hpp"
#include "MB/modbusUtils.hpp"
#include "rdbus/config/modbus/Register.hpp"
#include <numeric>

using namespace rdbus::config;
using namespace rdbus::config::modbus;

namespace rdbus::communication::modbus
{

static RequestDescriptions requestPlan( int slaveId, MB::utils::MBFunctionCode code, Slave::Registers registers )
{
    RequestDescriptions requestDescriptions;

    // Sort registers for easier address grouping
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

            // TODO: Will probably need to find a way how to read from coils too
            auto request = MB::ModbusRequest( slaveId, code, address, rawRegisterCount );
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

RequestDescriptions requestPlan( const Slave& slave )
{
    RequestDescriptions requestDescriptions;

    auto descriptions1 = requestPlan( slave.id, MB::utils::MBFunctionCode::ReadAnalogOutputHoldingRegisters, slave.outputRegisters );
    auto descriptions2 = requestPlan( slave.id, MB::utils::MBFunctionCode::ReadAnalogInputRegisters, slave.inputRegisters );

    requestDescriptions.insert( requestDescriptions.end(), descriptions1.begin(), descriptions1.end() );
    requestDescriptions.insert( requestDescriptions.end(), descriptions2.begin(), descriptions2.end() );

    return requestDescriptions;
}

} // namespace rdbus::communication::modbus
