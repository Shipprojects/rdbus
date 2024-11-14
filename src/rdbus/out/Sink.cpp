#include "Sink.hpp"

namespace rdbus::out
{

Sink::Sink( const std::map< ProcessorName, BufferType >& processorDescriptions )
{
    for ( const auto& [ processorName, bufferType ] : processorDescriptions )
    {
        processorStorage.insert( { processorName, Buffer< BufferProcessingData >( bufferType, processorName ) } );
    }
}

void Sink::put( const std::list< rdbus::Data >& list )
{
    std::lock_guard lock( mutex );
    rdbusDataStorage.add( list );
}

void Sink::put( const processing::Base::OutputList& list, ProcessorName processorName )
{
    std::lock_guard lock( mutex );
    processorStorage.at( processorName ).add( list );
}

nlohmann::json Sink::get( BufferTimePoint timePoint )
{
    std::lock_guard lock( mutex );
    return rdbusDataStorage.parseFrom( timePoint );
}

nlohmann::json Sink::get( BufferTimePoint timePoint, ProcessorName processorName )
{
    std::lock_guard lock( mutex );
    return processorStorage.at( processorName ).parseFrom( timePoint );
}

} // namespace rdbus::out
