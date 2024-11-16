#pragma once

#include "Buffer.hpp"
#include "rdbus/Data.hpp"
#include "rdbus/processing/Processor.hpp"
#include <nlohmann/json.hpp>

namespace rdbus::out
{

// A place where processed data from all managers come into a single place to be stored
// and retrieved for output
class Storage
{
private:
    using ProcessorName = processing::Name;

public:
    Storage( const std::map< ProcessorName, BufferType >& processorDescriptions );

    void put( const std::list< rdbus::Data >& );
    void put( const processing::Processor::OutputList&, ProcessorName );

    nlohmann::json get( BufferTimePoint ); // Returns parsed rdbus::Data
    nlohmann::json get( BufferTimePoint, ProcessorName ); // Returns parsed processor data

private:
    Buffer< rdbus::Data > rdbusDataStorage;

    // Each buffer is dedicated for a single data processor output
    std::map< ProcessorName, Buffer< ProcessingData > > processorStorage;
};

} // namespace rdbus::out
