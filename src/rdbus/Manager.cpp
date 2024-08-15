#include "Manager.hpp"
#include <chrono>
#include <thread>

namespace rdbus
{

Manager::Manager( Tasks tasks, Output output )
: tasks( std::move( tasks ) ),
  output( std::move( output ) )
{
}

std::optional< Data > Manager::run( std::unique_ptr< tasks::Task >& task )
{
    return task->run();
}


void Manager::run()
{
    std::list< rdbus::Data > list;
    for ( auto& task : tasks )
    {
        auto data = run( task );
        if ( data.has_value() )
        {
            list.emplace_back( std::move( data.value() ) );
        }
    }

    if ( !list.empty() )
    {
        output->send( list );
    }

    std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
}

} // namespace rdbus
