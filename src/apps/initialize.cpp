#include "initialize.hpp"
#include "Args.hpp"
#include "spdlog/common.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "version.hpp"
#include <argparse/argparse.hpp>
#include <exception>
#include <spdlog/spdlog.h>

using LogLevel = spdlog::level::level_enum;

static rdbus::config::Output toOutputConfig( bool toStdout, const std::string& ipPort );

Args parseArguments( int argc, char** argv )
{
    Args args;
    argparse::ArgumentParser parser( "rdbus", VERSION );

    parser.add_description( "Modbus serial reader and parser. Requires .json config file. You can either pass "
                            "the path to the config file or rdbus will search for '.rdbus.json' in current folder. "
                            "The arguments that you pass on command line take precedence over the arguments "
                            "in config file." );

    parser.add_epilog( "Note: rdbus runs with valid configuration only!" );

    parser.add_argument( "--config-dir" )
    .help( "path to directory containing *.rdbus.json configuration files" )
    .default_value( "." )
    .store_into( args.configDir );

    auto& outputGroup = parser.add_mutually_exclusive_group( true );

    bool toStdout = false;
    outputGroup.add_argument( "--stdout", "-s" )
    .help( "output to stdout" )
    .flag()
    .store_into( toStdout );

    std::string ipPort;
    outputGroup.add_argument( "--ip" )
    .help( "'ip:port' of HTTP server for output" )
    .store_into( ipPort );

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

    args.output = toOutputConfig( toStdout, ipPort );

    return args;
}

rdbus::config::Output toOutputConfig( bool toStdout, const std::string& ipPort )
{
    nlohmann::json jsonConfig;

    // Since all required checks for output settings are inside of config::Output
    // deserializer, then it is easier to build a JSON and parse it afterwards
    if ( toStdout )
    {
        jsonConfig[ "type" ] = "stdout";
    }
    else
    {
        const auto& ip = ipPort.substr( 0, ipPort.find( ':' ) );
        const auto& port = ipPort.substr( ipPort.find( ':' ) + 1 );

        jsonConfig[ "type" ] = "TCP/IP";
        jsonConfig[ "ip" ] = ip;
        jsonConfig[ "port" ] = std::stoi( port );
    }

    return jsonConfig;
}

void initializeLogger( LogLevel loglevel )
{
    auto logger = spdlog::stderr_logger_mt( "logger" );
    spdlog::set_default_logger( logger );
    spdlog::set_level( loglevel );
    spdlog::set_pattern( "[%Y-%m-%d %H:%M:%S:%e] [%l] %v" );
}
