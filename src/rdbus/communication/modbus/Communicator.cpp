#include "Communicator.hpp"
#include "Interpreter.hpp"
#include "RequestPlanner.hpp"
#include "rdbus/Data.hpp"

namespace rdbus::communication::modbus
{

Communicator::Communicator( const config::Serial& settings )
: adapter( settings )
{
}

rdbus::Data Communicator::request( const config::Slave& slave )
{
    const auto& requestDescriptions = requestPlan( slave );

    rdbus::Data data;
    data.deviceName = slave.name;
    for ( const auto& description : requestDescriptions )
    {
        const auto& response = adapter.send( description.request );

        const auto& timestamp = std::chrono::system_clock::now();
        const auto& fields = interpreter::parse( response, description.registers, timestamp );
        for ( const auto& field : fields )
        {
            data.fields.emplace_back( std::move( field ) );
        }
    }

    return data;
}

} // namespace rdbus::communication::modbus
