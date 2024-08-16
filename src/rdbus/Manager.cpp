#include "Manager.hpp"

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

void Manager::send( std::list< rdbus::Data > list )
{
    output->send( list );
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
        send( list );
    }
}

} // namespace rdbus
