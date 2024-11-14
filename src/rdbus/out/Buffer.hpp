#pragma once

#include "rdbus/processing/Base.hpp"
#include <concepts>
#include <deque>
#include <list>
#include <nlohmann/json.hpp>

namespace rdbus::out
{

enum BufferType
{
    Stream,
    Single
};

using BufferTimePoint = std::chrono::time_point< std::chrono::system_clock >;
using BufferProcessingData = std::shared_ptr< rdbus::processing::Base::Data >;

// A self-cleaning buffer that you can get all data from starting at your
// given time point
template < class DataType >
class Buffer
{
public:
    Buffer() requires std::same_as< DataType, rdbus::Data >;
    Buffer( BufferType type, processing::Name processorName ) requires std::same_as< DataType, BufferProcessingData >;

    void add( const std::list< DataType >& list );

    nlohmann::json parseFrom( const BufferTimePoint& lastTime ) const;

    using TimeDataPairs = std::deque< std::pair< BufferTimePoint, DataType > >;
    TimeDataPairs data;
    constexpr static auto maxBytes = 500'000;

private:
    void constrainSize();

    BufferType type;
    processing::Name processorName;
};

} // namespace rdbus::out
