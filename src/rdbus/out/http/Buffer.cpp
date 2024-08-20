#include "Buffer.hpp"

using namespace nlohmann;

namespace rdbus::out::http
{

void Buffer::add( const std::list< rdbus::Data >& list )
{
    for ( const auto& entry : list )
    {
        data.emplace_back( std::chrono::system_clock::now(), entry );
    }

    constrainSize();
}

json Buffer::parseFrom( const TimePoint& lastTime )
{
    // Find the data entry whos timepoint is bigger than the passed lastTime, because
    // the bigger the timepoint the later it came.
    // Note that the buffer will always be sorted relative according to the timepoints
    // of fields.
    const auto& begin = std::upper_bound( data.begin(), data.end(), lastTime,
                                          []( const TimePoint& value, const TimeDataPairs::value_type& pair )
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

void Buffer::constrainSize()
{
    // Pop front of the buffer while there is more than max amount stored there
    while ( data.size() * sizeof( rdbus::Data ) > maxBytes )
    {
        data.pop_front();
    }
}

} // namespace rdbus::out::http
