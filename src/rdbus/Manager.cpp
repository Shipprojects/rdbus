#include "Manager.hpp"

namespace rdbus
{

Manager::Manager( const std::string& name, Tasks tasks, Output output, Processors processors )
: name( name ),
  tasks( std::move( tasks ) ),
  output( output ),
  processors( std::move( processors ) )
{
}

const std::string& Manager::getName() const
{
    return name;
}

void Manager::send( const std::list< rdbus::Data >& list )
{
    output->send( list );
}

void Manager::send( const processing::Base::OutputList& list )
{
    output->send( list );
}

void Manager::run()
{
    std::list< rdbus::Data > list;
    for ( auto& task : tasks )
    {
        auto data = task->run();
        list.insert( list.end(), data.begin(), data.end() );
    }

    if ( !list.empty() )
    {
        send( list );
    }

    for ( auto& processor : processors )
    {
        send( processor->process( list ) );
    }
}

} // namespace rdbus
