#include "Modbus.hpp"
#include <chrono>

namespace rdbus::tasks
{

Modbus::Modbus( const config::Slave& slave, const Communicator& communicator )
: slave( slave ),
  com( communicator )
{
}

std::optional< Data > Modbus::run()
{
    const auto now = std::chrono::steady_clock::now();
    const auto diff = now - lastRun;
    const auto pauseMs = std::chrono::duration_cast< std::chrono::milliseconds >( diff );
    std::optional< rdbus::Data > data;

    if ( pauseMs >= slave.pollTimeMs )
    {
        lastRun = now;
        data = com->request( slave );
    }

    return data;
}

} // namespace rdbus::tasks
