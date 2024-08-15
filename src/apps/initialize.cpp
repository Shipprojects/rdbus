#include "initialize.hpp"
#include "Args.hpp"
#include "rdbus/communication/modbus/Communicator.hpp"
#include "rdbus/config/Config.hpp"
#include "rdbus/out/pipe/Pipe.hpp"
#include "rdbus/tasks/Modbus.hpp"
#include <argparse/argparse.hpp>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <version/version.hpp>

Args parseArguments( int argc, char** argv )
{
    Args args;
    argparse::ArgumentParser parser( "rdbus", VERSION );

    parser.add_description( "Modbus serial reader and parser. Requires .json config file. You can either pass "
                            "the path to the config file or rdbus will search for '.rdbus.json' in current folder. "
                            "The arguments that you pass on command line take precedence over the arguments "
                            "in config file." );

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

    // args.add_argument( "--log-level", "-l" ).help( "Log level" );

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

    return args;
}

void initializeLogger()
{
}

static void validateConfig( const rdbus::config::Config& )
{
    // ADD VALIDATION OF CONFIG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // ADD VALIDATION OF CONFIG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // ADD VALIDATION OF CONFIG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // ADD VALIDATION OF CONFIG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // ADD VALIDATION OF CONFIG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // ADD VALIDATION OF CONFIG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // ADD VALIDATION OF CONFIG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // ADD VALIDATION OF CONFIG !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
    validateConfig( config );

    return config;
}

rdbus::Manager::Tasks initializeTasks( const rdbus::config::Config& config )
{
    rdbus::Manager::Tasks tasks;

    auto communicator = std::make_shared< rdbus::communication::modbus::Communicator >( config.serial );
    for ( const auto& slave : config.slaves )
    {
        tasks.emplace_back( std::make_unique< rdbus::tasks::Modbus >( slave, communicator ) );
    }

    return tasks;
}

rdbus::Manager::Output initializeOutput( const rdbus::config::Config& config )
{
    auto output = std::make_unique< rdbus::out::pipe::Pipe >();

    return output;
}
