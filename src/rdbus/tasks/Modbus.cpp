#include "Modbus.hpp"
#include <chrono>

namespace rdbus::tasks
{

Modbus::Modbus( const config::Slave& slave, rdbus::communication::modbus::Communicator& communicator )
: slave( slave ),
  com( communicator )
{
}

std::list< Data > Modbus::run()
{
    const auto now = std::chrono::steady_clock::now();
    const auto diff = now - lastRun;
    const auto pauseMs = std::chrono::duration_cast< std::chrono::milliseconds >( diff );
    std::list< rdbus::Data > list;

    if ( pauseMs >= slave.pollTimeMs )
    {
        auto data = com.request( slave );
        list.emplace_back( std::move( data ) );
        lastRun = now;
    }

    return list;
}

} // namespace rdbus::tasks
