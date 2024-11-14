#include "Storage.hpp"

namespace rdbus::out
{

Storage::Storage( const std::map< ProcessorName, BufferType >& processorDescriptions )
{
    for ( const auto& [ processorName, bufferType ] : processorDescriptions )
    {
        processorStorage.insert( { processorName, Buffer< ProcessingData >( bufferType, processorName ) } );
    }
}

void Storage::put( const std::list< rdbus::Data >& list )
{
    rdbusDataStorage.add( list );
}

void Storage::put( const processing::Base::OutputList& list, ProcessorName processorName )
{
    processorStorage.at( processorName ).add( list );
}

nlohmann::json Storage::get( BufferTimePoint timePoint )
{
    return rdbusDataStorage.parseFrom( timePoint );
}

nlohmann::json Storage::get( BufferTimePoint timePoint, ProcessorName processorName )
{
    return processorStorage.at( processorName ).parseFrom( timePoint );
}

} // namespace rdbus::out
