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

std::list< Data > Manager::run( std::unique_ptr< tasks::Task >& task )
{
    return task->run();
}

void Manager::send( const std::list< rdbus::Data >& list )
{
    output->send( list );
}

void Manager::run()
{
    std::list< rdbus::Data > list;
    for ( auto& task : tasks )
    {
        auto data = run( task );
        list.insert( list.end(), data.begin(), data.end() );
    }

    if ( !list.empty() )
    {
        send( list );
    }
}

} // namespace rdbus
