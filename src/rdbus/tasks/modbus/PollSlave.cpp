#include "PollSlave.hpp"
#include <chrono>

namespace rdbus::tasks::modbus
{

PollSlave::PollSlave( const config::Slave& slave, const Communicator& communicator )
: slave( slave ),
  com( communicator )
{
}

std::optional< Data > PollSlave::run()
{
    const auto now = std::chrono::steady_clock::now();
    const auto diff = now - lastRun;
    const auto timeSinceLastExecution = std::chrono::duration_cast< std::chrono::milliseconds >( diff );
    std::optional< rdbus::Data > data;

    if ( timeSinceLastExecution >= slave.pollTimeMs )
    {
        lastRun = now;
        data = com->request( slave );
    }

    return data;
}

} // namespace rdbus::tasks