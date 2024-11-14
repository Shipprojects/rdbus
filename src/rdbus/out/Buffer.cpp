#include "Buffer.hpp"
#include "rdbus/processing/Base.hpp"
#include "rdbus/processing/limits/Data.hpp"
#include <memory>

using namespace nlohmann;

namespace rdbus::out
{

template < class DataType >
Buffer< DataType >::Buffer() requires std::same_as< DataType, rdbus::Data >
: type( BufferType::Stream ),
  processorName( processing::Name::Read )
{
}

template < class DataType >
Buffer< DataType >::Buffer( BufferType type, processing::Name processorName ) requires std::same_as< DataType, BufferProcessingData >
: type( type ),
  processorName( processorName )
{
}

template < class DataType >
void Buffer< DataType >::add( const std::list< DataType >& list )
{
    if ( type == BufferType::Single )
    {
        data.clear();
    }

    const auto& now = std::chrono::system_clock::now();
    for ( const auto& entry : list )
    {
        data.emplace_back( now, entry );
    }

    if ( type == BufferType::Stream )
    {
        constrainSize();
    }
}

template <>
json Buffer< rdbus::Data >::parseFrom( const BufferTimePoint& lastTime ) const
{
    // Find the data entry whos timepoint is bigger than the passed lastTime, because
    // the bigger the timepoint the later it came.
    // Note that the buffer will always be sorted relative according to the timepoints
    // of fields.
    const auto& begin = std::upper_bound( data.begin(), data.end(), lastTime,
                                          []( const BufferTimePoint& value, const TimeDataPairs::value_type& pair )
                                          {
                                              return value < pair.first;
                                          } );

    auto json = json::array();
    std::for_each( begin, data.end(),
                   [ & ]( const TimeDataPairs::value_type& pair )
                   {
                       json.push_back( pair.second );
                   } );

    return json;
}

template <>
json Buffer< std::shared_ptr< rdbus::processing::Base::Data > >::parseFrom( const BufferTimePoint& lastTime ) const
{
    // Find the data entry whos timepoint is bigger than the passed lastTime, because
    // the bigger the timepoint the later it came.
    // Note that the buffer will always be sorted relative according to the timepoints
    // of fields.
    const auto& begin = std::upper_bound( data.begin(), data.end(), lastTime,
                                          []( const BufferTimePoint& value, const TimeDataPairs::value_type& pair )
                                          {
                                              return value < pair.first;
                                          } );

    auto json = json::array();
    std::for_each( begin, data.end(),
                   [ & ]( const TimeDataPairs::value_type& pair )
                   {
                       switch ( processorName )
                       {
                           case processing::Name::Limits:
                           default:
                           {
                               const auto limits = std::dynamic_pointer_cast< processing::limits::Data >( pair.second );
                               json.push_back( *limits );
                           }
                       }
                   } );

    return json;
}

template < class DataType >
void Buffer< DataType >::constrainSize()
{
    // Pop front of the buffer while there is more than max amount stored there
    while ( data.size() * sizeof( rdbus::Data ) > maxBytes )
    {
        data.pop_front();
    }
}

template class Buffer< rdbus::Data >;
template class Buffer< BufferProcessingData >;

} // namespace rdbus::out
