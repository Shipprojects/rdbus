#include "Buffer.hpp"
#include "rdbus/processing/Processor.hpp"
#include <memory>

using namespace nlohmann;

namespace rdbus::out
{

template < class DataType >
Buffer< DataType >::Buffer() requires std::same_as< DataType, rdbus::Data >
: type( BufferType::Stream )
{
}

template < class DataType >
Buffer< DataType >::Buffer( BufferType type ) requires std::same_as< DataType, ProcessingData >
: type( type )
{
}

template <>
void Buffer< std::shared_ptr< rdbus::processing::Processor::Data > >::removeIntersection( const std::list< std::shared_ptr< rdbus::processing::Processor::Data > >& list )
{
    for ( const auto& entry : list )
    {
        auto it = std::remove_if( data.begin(), data.end(),
                                  [ & ]( const auto& element )
                                  {
                                      return element.second->getDeviceName() == entry->getDeviceName();
                                  } );
        data.erase( it, data.end() );
    }
}

template <>
void Buffer< rdbus::Data >::removeIntersection( const std::list< rdbus::Data >& list )
{
    for ( const auto& entry : list )
    {
        auto it = std::remove_if( data.begin(), data.end(),
                                  [ & ]( const auto& element )
                                  {
                                      return element.second.deviceName == entry.deviceName;
                                  } );
        data.erase( it, data.end() );
    }
}

template < class DataType >
void Buffer< DataType >::add( const std::list< DataType >& list )
{
    // Erase entries which are going to be updated with new data
    if ( type == BufferType::Single )
    {
        removeIntersection( list );
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
json Buffer< std::shared_ptr< rdbus::processing::Processor::Data > >::toJson( TimeDataPairs::const_iterator begin ) const
{
    auto json = json::array();
    std::for_each( begin, data.end(),
                   [ & ]( const TimeDataPairs::value_type& pair )
                   {
                       json.push_back( pair.second->toJson() );
                   } );

    return json;
}

template <>
json Buffer< rdbus::Data >::toJson( TimeDataPairs::const_iterator begin ) const
{
    auto json = json::array();
    std::for_each( begin, data.end(),
                   [ & ]( const TimeDataPairs::value_type& pair )
                   {
                       json.push_back( pair.second );
                   } );

    return json;
}

template < class DataType >
json Buffer< DataType >::parseFrom( const BufferTimePoint& lastTime ) const
{
    auto begin = data.begin();
    if ( type == BufferType::Stream )
    {
        // Find the data entry whos timepoint is bigger than the passed lastTime, because
        // the bigger the timepoint the later it came.
        // Note that the buffer will always be sorted relative to the timepoints of fields.
        begin = std::upper_bound( data.begin(), data.end(), lastTime,
                                  []( const BufferTimePoint& value, const TimeDataPairs::value_type& pair )
                                  {
                                      return value < pair.first;
                                  } );
    }

    return toJson( begin );
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
template class Buffer< ProcessingData >;

} // namespace rdbus::out
