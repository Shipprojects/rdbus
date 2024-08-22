#include "Initialize.hpp"
#include "Manager.hpp"
#include "communication/modbus/Communicator.hpp"
#include "communication/nmea/Communicator.hpp"
#include "config/Config.hpp"
#include "out/http/HTTP.hpp"
#include "out/pipe/Pipe.hpp"
#include "rdbus/config/Output.hpp"
#include "tasks/modbus/PollSlave.hpp"
#include "tasks/nmea/Listen.hpp"
#include <filesystem>
#include <stdexcept>

namespace rdbus
{

std::list< config::Config > initializeConfigs( const std::string& configDir )
{
    std::list< nlohmann::json > jsonList;

    // Get all configs from files that end with .rdbus.json
    const std::string extension( ".rdbus.json" );
    for ( const auto& it : std::filesystem::directory_iterator( configDir ) )
    {
        const auto& path = it.path().string();
        if ( path.length() >= extension.length() &&
             path.compare( path.length() - extension.length(), extension.length(), extension ) == 0 )
        {
            std::ifstream file( path );
            if ( !file.good() )
            {
                throw std::runtime_error( "Failed to open config file at " + path + "!" );
            }

            jsonList.emplace_back( nlohmann::json::parse( file ) );
        }
    }

    if ( jsonList.empty() )
    {
        throw std::runtime_error( "No '*.rdbus.json' configuration files found in " + configDir + "!" );
    }

    return std::list< config::Config >( jsonList.begin(), jsonList.end() );
}

Manager::Output initializeOutput( const config::Output& output )
{
    if ( output.type == config::Output::TCP_IP )
    {
        SPDLOG_INFO( "Outputting using TCP/IP server" );
        return std::make_shared< out::http::HTTP >( output );
    }
    else if ( output.type == config::Output::Stdout )
    {
        SPDLOG_INFO( "Outputting to STDOUT" );
        return std::make_shared< out::pipe::Pipe >();
    }

    // Throw if we came here
    throw std::runtime_error( "Unknown output type!" );
}

static Manager::Tasks initializeTasks( const config::Config& config )
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

std::list< rdbus::Manager > initializeManagers( const std::list< rdbus::config::Config >& configs, std::shared_ptr< out::Output > output )
{
    std::list< rdbus::Manager > managers;
    for ( const auto& config : configs )
    {
        managers.emplace_back( config.serial.path, initializeTasks( config ), output );
    }

    return managers;
}


} // namespace rdbus
