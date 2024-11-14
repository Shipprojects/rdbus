#pragma once

#include "Buffer.hpp"
#include "rdbus/Data.hpp"
#include "rdbus/processing/Base.hpp"
#include <nlohmann/json.hpp>

namespace rdbus::out
{

// A place where processed data from all managers come into a single place to be stored
// and retrieved for output
class Sink
{
private:
    using ProcessorName = processing::Name;

public:
    Sink( const std::map< ProcessorName, BufferType >& processorDescriptions );

    void put( const std::list< rdbus::Data >& );
    void put( const processing::Base::OutputList&, ProcessorName );

    nlohmann::json get( BufferTimePoint ); // Returns parsed rdbus::Data
    nlohmann::json get( BufferTimePoint, ProcessorName ); // Returns parsed processor data

private:
    Buffer< rdbus::Data > rdbusDataStorage;
    std::map< ProcessorName, Buffer< BufferProcessingData > > processorStorage;

    std::mutex mutex;
};

} // namespace rdbus::out
