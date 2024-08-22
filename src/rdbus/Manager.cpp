#include "Manager.hpp"

namespace rdbus
{

Manager::Manager( const std::string& name, Tasks tasks, Output output )
: name( name ),
  tasks( std::move( tasks ) ),
  output( output )
{
}

const std::string& Manager::getName() const
{
    return name;
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
