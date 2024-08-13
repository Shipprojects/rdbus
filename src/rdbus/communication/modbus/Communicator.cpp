#include "Communicator.hpp"
#include "RequestPlanner.hpp"
#include "rdbus/Data.hpp"

namespace rdbus::communication::modbus
{

Communicator::Communicator( const config::Serial& settings )
: adapter( settings )
{
}

std::list< rdbus::Data > Communicator::request( const config::Slave& slave )
{
    const auto& requestDescriptions = requestPlan( slave );

    for ( const auto& description : requestDescriptions )
    {
        const auto& response = adapter.send( description.request );
    }

    return {};
}

} // namespace rdbus::communication::modbus
