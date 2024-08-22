#include "Initialize.hpp"
#include "Manager.hpp"
#include "communication/modbus/Communicator.hpp"
#include "communication/nmea/Communicator.hpp"
#include "config/Config.hpp"
#include "out/http/HTTP.hpp"
#include "out/pipe/Pipe.hpp"
#include "tasks/modbus/PollSlave.hpp"
#include "tasks/nmea/Listen.hpp"

namespace rdbus
{

Manager::Tasks initializeTasks( const config::Config& config )
{
    Manager::Tasks tasks;

    if ( config.protocol == "nmea" )
    {
        auto communicator = std::make_shared< communication::nmea::Communicator >( config.serial, std::make_unique< communication::OSWrapper >() );
        tasks.emplace_back( std::make_unique< tasks::nmea::Listen >( config.nmea, communicator ) );
    }
    else if ( config.protocol == "modbus" )
    {
        auto communicator = std::make_shared< communication::modbus::Communicator >( config.serial, std::make_unique< communication::OSWrapper >() );
        for ( const auto& slave : config.modbus.slaves )
        {
            // 1 slave == 1 task
            tasks.emplace_back( std::make_unique< tasks::modbus::PollSlave >( slave, communicator ) );
        }
    }

    return tasks;
}

Manager::Output initializeOutput( const config::Config& config )
{
    if ( config.output.type == config::Output::TCP_IP )
    {
        SPDLOG_INFO( "Starting TCP/IP server" );
        return std::make_unique< out::http::HTTP >( config.output );
    }
    else if ( config.output.type == config::Output::Stdout )
    {
        SPDLOG_INFO( "Outputting to STDOUT" );
        return std::make_unique< out::pipe::Pipe >();
    }

    // Throw if we came here
    throw std::runtime_error( "Unknown output type!" );
}

} // namespace rdbus
