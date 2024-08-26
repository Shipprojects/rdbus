#include "PollSlave.hpp"
#include <chrono>

namespace rdbus::tasks::modbus
{

PollSlave::PollSlave( const config::modbus::Slave& slave, const Communicator& communicator )
: slave( slave ),
  com( communicator )
{
}

std::list< Data > PollSlave::run()
{
    const auto now = std::chrono::steady_clock::now();
    const auto diff = now - lastRun;
    const auto timeSinceLastExecution = std::chrono::duration_cast< std::chrono::milliseconds >( diff );
    std::list< rdbus::Data > data;

    if ( timeSinceLastExecution >= slave.pollTimeMs )
    {
        lastRun = now;
        data.emplace_back( com->request( slave ) );
    }

    return data;
}

} // namespace rdbus::tasks::modbus
