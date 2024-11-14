#include "initialize.hpp"
#include "rdbus/Initialize.hpp"
#include "rdbus/Manager.hpp"
#include <atomic>
#include <chrono>
#include <csignal>
#include <spdlog/spdlog.h>
#include <thread>

// The return value of the program. Atomic because it is shared between
// threads so it would be possible to set result to 1 in case of thread
// failure.
static volatile std::atomic< int > result = 0;

// The variable which allows 'infinite' loops. Shared between threads.
static volatile std::atomic< bool > keepRunning = true;
void signalHandler( int signum )
{
    // Interrupt from keyboard, Control-C
    if ( signum == SIGINT )
    {
        keepRunning = false;
    }
}

void stop( std::list< std::thread >& threads )
{
    // Wait for threads to finish
    for ( auto& thread : threads )
    {
        thread.join();
    }
}

void start( std::list< std::thread >& threads, std::list< rdbus::Manager >& managers )
{
    for ( auto& manager : managers )
    {
        threads.emplace_back(
        [ & ]()
        {
            SPDLOG_INFO( "Starting manager of " + manager.getName() );
            while ( keepRunning )
            {
                // This try-catch clause is meant to catch exceptions on actual runtime
                // (i.e. after setup). In general most of the exceptions must be handled by
                // manager, however if some kind of exception comes from it then it means
                // that some critical error occurred and the program shall exit.
                try
                {
                    manager.run();
                }
                catch ( const std::exception& e )
                {
                    SPDLOG_CRITICAL( e.what() );
                    keepRunning = false;
                    result = 1;
                }
                catch ( ... )
                {
                    SPDLOG_CRITICAL( "Unknown exception occurred!" );
                    keepRunning = false;
                    result = 1;
                }
                std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
            }
        } );
    }
}

int main( int argc, char** argv )
{
    signal( SIGINT, &signalHandler );

    std::list< std::thread > threads;
    std::list< rdbus::Manager > managers;

    // This try-catch clause is meant to catch exceptions on configuration
    // time. In case of invalid configuration the program will exit.
    try
    {
        const auto& args = parseArguments( argc, argv );
        initializeLogger( args.logLevel );
        SPDLOG_INFO( "Starting" );
        const auto& configs = rdbus::initializeConfigs( args.configDir );

        auto output = rdbus::initializeOutput( args.output, configs );
        managers = rdbus::initializeManagers( configs, output );
    }
    catch ( const std::exception& e )
    {
        SPDLOG_CRITICAL( e.what() );
        result = 1;
    }
    catch ( ... )
    {
        SPDLOG_CRITICAL( "Unknown exception occurred!" );
        result = 1;
    }

    // Start threads if everything is OK
    if ( result == 0 )
    {
        start( threads, managers );
    }
    stop( threads );

    SPDLOG_INFO( "Exiting" );
    return result;
}
