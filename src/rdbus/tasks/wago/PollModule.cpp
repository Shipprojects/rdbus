#include "PollModule.hpp"
#include <chrono>

using namespace rdbus::config::wago;

namespace rdbus::tasks::wago
{

PollModule::PollModule( const Module& module, const Communicator& communicator )
: module( module ),
  com( communicator )
{
}

std::list< Data > PollModule::run()
{
    const auto now = std::chrono::steady_clock::now();
    const auto diff = now - lastRun;
    const auto timeSinceLastExecution = std::chrono::duration_cast< std::chrono::milliseconds >( diff );
    std::list< rdbus::Data > data;

    if ( timeSinceLastExecution >= module.pollTimeMs )
    {
        lastRun = now;
        data.emplace_back( com->request( module ) );
    }

    return data;
}

} // namespace rdbus::tasks::modbus
