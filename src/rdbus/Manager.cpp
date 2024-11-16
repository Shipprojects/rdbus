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
        output->send( list );
    }

    for ( auto& processor : processors )
    {
        output->send( processor->process( list ), processor->getName() );
    }
}

} // namespace rdbus
