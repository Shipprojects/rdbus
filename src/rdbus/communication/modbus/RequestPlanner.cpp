#include "RequestPlanner.hpp"
#include "MB/modbusRequest.hpp"
#include "MB/modbusUtils.hpp"
#include "config/Register.hpp"

namespace communication
{

namespace modbus
{

using namespace config;
std::list< MB::ModbusRequest > requestPlan( const Slave& slave )
{
    std::list< MB::ModbusRequest > requests;

    // Sort registers for easier address grouping
    auto registers = slave.registers;
    registers.sort( []( const Register& left, const Register& right )
                    { return left.address < right.address; } );


    // Starting address of register group
    int address = registers.begin()->address;
    int requestRegisterCount = 0;
    for ( auto it = registers.begin(); it != registers.end(); it++ )
    {
        requestRegisterCount++;

        // If next register is not directly adjacent to the current one or next
        // register is the last register
        if ( ( it->address + it->byteOrder.size() != std::next( it )->address ) ||
             ( std::next( it ) == registers.end() ) )
        {
            // TODO: Will probably need to find a way how to read from coils too, otherwise the only reading
            // that is possible now is from holding registers due to hardcoded function code
            requests.emplace_back( slave.address, MB::utils::MBFunctionCode::ReadAnalogOutputHoldingRegisters,
                                   address, requestRegisterCount );

            requestRegisterCount = 0;

            if ( std::next( it ) != registers.end() )
            {
                // Store the new group starting address
                address = std::next( it )->address;
            }
        }
    }

    return requests;
}

} // namespace modbus

} // namespace communication
