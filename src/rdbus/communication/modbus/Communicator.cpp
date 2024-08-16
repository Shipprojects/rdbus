#include "Communicator.hpp"
#include "Interpreter.hpp"
#include "RequestPlanner.hpp"
#include "rdbus/Data.hpp"

namespace rdbus::communication::modbus
{

Communicator::Communicator( const config::Serial& settings, std::unique_ptr< OS > os )
: adapter( settings, std::move( os ) )
{
}

rdbus::Data Communicator::request( const config::Slave& slave )
{
    // Contains planned list of requests and a separate list of registers which
    // match the order of requests
    const auto& requestDescriptions = requestPlan( slave );

    rdbus::Data data;
    data.deviceName = slave.name;
    for ( const auto& description : requestDescriptions )
    {
        const auto& response = adapter.send( description.request );

        const auto& timestamp = std::chrono::system_clock::now();
        // Parse response to data fields
        const auto& fields = interpreter::parse( response, description.registers, timestamp );
        for ( const auto& field : fields )
        {
            data.fields.emplace_back( std::move( field ) );
        }
    }

    return data;
}

} // namespace rdbus::communication::modbus
