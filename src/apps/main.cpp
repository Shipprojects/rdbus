#include "initialize.hpp"
#include "rdbus/Manager.hpp"
#include <atomic>
#include <csignal>
#include <iostream>

static volatile std::atomic< bool > keepRunning = true;
void signalHandler( int signum )
{
    // SIGINT is Ctrl+C from terminal too
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
        // initializeLogger( args.logLevel );
        const auto& config = initializeConfig( args );

        auto tasks = initializeTasks( config );
        auto output = initializeOutput( config );

        rdbus::Manager manager( std::move( tasks ), std::move( output ) );
        while ( keepRunning )
        {
            manager.run();
        }
    }
    catch ( const std::exception& e )
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch ( ... )
    {
        std::cerr << "Unknown exception occured!" << std::endl;
        return 1;
    }

    return 0;
}
