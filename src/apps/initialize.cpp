#include "initialize.hpp"
#include "Args.hpp"
#include "rdbus/communication/OSWrapper.hpp"
#include "rdbus/communication/modbus/Communicator.hpp"
#include "rdbus/config/Config.hpp"
#include "rdbus/out/pipe/Pipe.hpp"
#include "rdbus/tasks/Modbus.hpp"
#include "spdlog/common.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "version.hpp"
#include <argparse/argparse.hpp>
#include <exception>
#include <fstream>
#include <spdlog/spdlog.h>
#include <stdexcept>

using LogLevel = spdlog::level::level_enum;

Args parseArguments( int argc, char** argv )
{
    Args args;
    argparse::ArgumentParser parser( "rdbus", VERSION );

    parser.add_description( "Modbus serial reader and parser. Requires .json config file. You can either pass "
                            "the path to the config file or rdbus will search for '.rdbus.json' in current folder. "
                            "The arguments that you pass on command line take precedence over the arguments "
                            "in config file." );

    parser.add_epilog( "Note: rdbus runs with valid configuration only. Command line arguments do not replace the "
                       "config file!" );

    parser.add_argument( "--config" )
    .help( "path to configuration JSON" )
    .default_value( ".rdbus.json" )
    .store_into( args.configPath );

    parser.add_argument( "--device" )
    .help( "path to serial port" )
    .store_into( args.devPath );

    parser.add_argument( "--stdout", "-s" )
    .help( "force output to stdout" )
    .flag()
    .store_into( args.forceStdout );

    parser.add_argument( "--log" )
    .help( "set log level" )
    .choices( "crit", "err", "warn", "info", "off" )
    .default_value( "warn" );

    try
    {
        parser.parse_args( argc, argv );
    }
    catch ( const std::exception& err )
    {
        std::cerr << err.what() << std::endl;
        std::cerr << parser;
        std::rethrow_exception( std::current_exception() );
    }

    const std::map< std::string, LogLevel > stringToLevelMap = {
        { "crit", LogLevel::critical },
        { "err", LogLevel::err },
        { "warn", LogLevel::warn },
        { "info", LogLevel::info },
        { "off", LogLevel::off },
    };
    args.logLevel = stringToLevelMap.at( parser.get( "--log" ) );

    return args;
}

void initializeLogger( LogLevel loglevel )
{
    auto logger = spdlog::stderr_logger_mt( "logger" );
    spdlog::set_default_logger( logger );
    spdlog::set_level( loglevel );
    spdlog::set_pattern( "[%Y-%m-%d %H:%M:%S:%e] [%l] %v" );
}

rdbus::config::Config initializeConfig( const Args& args )
{
    std::ifstream file( args.configPath );
    if ( !file.good() )
    {
        throw std::runtime_error( "Failed to open config file at " + args.configPath + "!" );
    }

    // Preprocess json with command line arguments
    auto jsonConfig = nlohmann::json::parse( file );

    // Insert stdout output into json
    if ( args.forceStdout && jsonConfig.contains( "output" ) && jsonConfig.at( "output" ).contains( "type" ) )
    {
        jsonConfig[ "output" ][ "type" ] = "stdout";
    }

    // Insert device path into json
    if ( !args.devPath.empty() && jsonConfig.contains( "serial" ) && jsonConfig.at( "serial" ).contains( "path" ) )
    {
        jsonConfig[ "serial" ][ "path" ] = args.devPath;
    }

    const rdbus::config::Config& config = jsonConfig;

    return config;
}

rdbus::Manager::Tasks initializeTasks( const rdbus::config::Config& config )
{
    rdbus::Manager::Tasks tasks;

    auto communicator = std::make_shared< rdbus::communication::modbus::Communicator >( config.serial, std::make_unique< rdbus::communication::OSWrapper >() );
    for ( const auto& slave : config.slaves )
    {
        // 1 slave == 1 task
        tasks.emplace_back( std::make_unique< rdbus::tasks::Modbus >( slave, communicator ) );
    }

    return tasks;
}

rdbus::Manager::Output initializeOutput( const rdbus::config::Config& )
{
    auto output = std::make_unique< rdbus::out::pipe::Pipe >();

    return output;
}
