#include "initialize.hpp"
#include "rdbus/Manager.hpp"
#include "rdbus/config/exception.hpp"
#include <atomic>
#include <csignal>
#include <spdlog/spdlog.h>

static volatile std::atomic< bool > keepRunning = true;
void signalHandler( int signum )
{
    // Interrupt from keyboard, Control-C
    if ( signum == SIGINT )
    {
        keepRunning = false;
    }
}

int main( int argc, char** argv )
{
    signal( SIGINT, &signalHandler );

    try
    {
        const auto& args = parseArguments( argc, argv );
        initializeLogger( args.logLevel );
        SPDLOG_INFO( "Starting" );
        const auto& config = initializeConfig( args );

        auto tasks = initializeTasks( config );
        auto output = initializeOutput( config );

        rdbus::Manager manager( std::move( tasks ), std::move( output ) );
        SPDLOG_INFO( "Entering loop" );
        while ( keepRunning )
        {
            manager.run();
        }
    }
    catch ( const rdbus::config::ParseException& e )
    {
        SPDLOG_CRITICAL( "Config file parse exception - " + std::string( e.what() ) );
    }
    catch ( const std::exception& e )
    {
        SPDLOG_CRITICAL( "Exception - " + std::string( e.what() ) );
        return 1;
    }
    catch ( ... )
    {
        SPDLOG_CRITICAL( "Unknown exception occured!" );
        return 1;
    }

    SPDLOG_INFO( "Exiting" );
    return 0;
}
