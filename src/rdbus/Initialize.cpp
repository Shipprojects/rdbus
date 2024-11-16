#include "Initialize.hpp"
#include "Manager.hpp"
#include "communication/modbus/Communicator.hpp"
#include "communication/nmea/Communicator.hpp"
#include "communication/wago/Communicator.hpp"
#include "config/Config.hpp"
#include "out/http/HTTP.hpp"
#include "out/pipe/Pipe.hpp"
#include "rdbus/Exception.hpp"
#include "rdbus/config/Output.hpp"
#include "rdbus/config/processors/Processors.hpp"
#include "rdbus/processing/Processor.hpp"
#include "rdbus/processing/limits/wago/Processor.hpp"
#include "tasks/modbus/PollSlave.hpp"
#include "tasks/nmea/Listen.hpp"
#include "tasks/wago/PollModule.hpp"
#include <filesystem>

namespace rdbus
{

std::list< config::Config > initializeConfigs( const std::string& configDir )
{
    std::list< nlohmann::json > jsonList;

    // Parse all configuration files that end with .rdbus.json
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
                throw Exception( "Failed to open config file at " + path + "!" );
            }

            jsonList.emplace_back( nlohmann::json::parse( file ) );
        }
    }

    if ( jsonList.empty() )
    {
        throw Exception( "No '*.rdbus.json' configuration files found in " + configDir + "!" );
    }

    return std::list< config::Config >( jsonList.begin(), jsonList.end() );
}

static std::map< processing::Name, out::BufferType > processorDescriptions( const std::list< rdbus::config::Config >& configs )
{
    std::map< processing::Name, out::BufferType > result;

    for ( const auto& config : configs )
    {
        if ( config.processors.limits.has_value() )
        {
            result.insert( { processing::Name::Limits, out::BufferType::Single } );
        }
    }

    return result;
}

Manager::Output initializeOutput( const config::Output& output, const std::list< rdbus::config::Config >& configs )
{
    if ( output.type == config::Output::TCP_IP )
    {
        SPDLOG_INFO( "Outputting using TCP/IP server" );
        return std::make_shared< out::http::HTTP >( *output.address, processorDescriptions( configs ) );
    }
    else if ( output.type == config::Output::Stdout )
    {
        SPDLOG_INFO( "Outputting to STDOUT" );
        return std::make_shared< out::pipe::Pipe >();
    }

    // Throw if we came here
    throw Exception( "Unknown output type!" );
}

static Manager::Tasks initializeTasks( const config::Config& config )
{
    Manager::Tasks tasks;

    if ( config.protocol == "nmea" )
    {
        throwIf( !config.serial.has_value(), "'serial' configuration required for 'protocol' nmea!" );

        auto communicator = std::make_shared< communication::nmea::Communicator >( *config.serial, std::make_unique< communication::OSWrapper >() );
        tasks.emplace_back( std::make_unique< tasks::nmea::Listen >( config.nmea, communicator ) );
    }
    else if ( config.protocol == "modbus" )
    {
        throwIf( !config.serial.has_value(), "'serial' configuration required for 'protocol' modbus!" );

        auto communicator = std::make_shared< communication::modbus::Communicator >( *config.serial, std::make_unique< communication::OSWrapper >() );
        for ( const auto& slave : config.modbus.slaves )
        {
            // 1 slave == 1 task
            tasks.emplace_back( std::make_unique< tasks::modbus::PollSlave >( slave, communicator ) );
        }
    }
    else if ( config.protocol == "wago" )
    {
        throwIf( !config.address.has_value(), "'address' configuration required for 'protocol' wago!" );

        auto communicator = std::make_shared< communication::wago::Communicator >( *config.address );
        for ( const auto& module : config.wago.modules )
        {
            // 1 module == 1 task
            tasks.emplace_back( std::make_unique< tasks::wago::PollModule >( module, communicator ) );
        }
    }

    return tasks;
}

static std::list< std::unique_ptr< processing::Processor > > initializeProcessors( const config::Config& config )
{
    Manager::Processors processors;

    if ( config.processors.limits.has_value() )
    {
        processors.emplace_back( std::make_unique< processing::limits::wago::Processor >( *config.processors.limits ) );
    }

    return processors;
}

std::list< Manager > initializeManagers( const std::list< config::Config >& configs, std::shared_ptr< out::Output > output )
{
    std::list< Manager > managers;
    for ( const auto& config : configs )
    {
        std::string managerName;
        if ( config.serial.has_value() )
        {
            managerName = config.serial->path;
        }
        else if ( config.address.has_value() )
        {
            managerName = config.address->ip + ':' + std::to_string( config.address->port );
        }
        managers.emplace_back( managerName, initializeTasks( config ), output, initializeProcessors( config ) );
    }

    return managers;
}

} // namespace rdbus
