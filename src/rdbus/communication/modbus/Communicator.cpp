#include "Communicator.hpp"
#include "RequestPlanner.hpp"

namespace communication::modbus
{

Communicator::Communicator( const config::Serial& settings )
: adapter( settings )
{
}

void Communicator::request( const config::Slave& slave )
{
    const auto& requests = requestPlan( slave );

    for ( const auto& request : requests )
    {
        const auto& response = adapter.send( request );
    }
}

} // namespace communication::modbus
