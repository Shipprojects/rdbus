#include "initialize.hpp"
#include "rdbus/Manager.hpp"
#include <atomic>
#include <chrono>
#include <csignal>
#include <spdlog/spdlog.h>
#include <thread>

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
            std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
        }
    }
    catch ( const std::exception& e )
    {
        SPDLOG_CRITICAL( e.what() );
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
