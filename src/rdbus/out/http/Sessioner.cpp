#include "Sessioner.hpp"
#include <chrono>
#include <list>
#include <string>

namespace rdbus::out::http
{

Sessioner::Exception::Exception( const std::string& what )
: std::runtime_error( "Cookie exception - " + what )
{
}

int Sessioner::create( const TimePoint& newAccessTime )
{
    std::lock_guard lock( mutex );

    int lastId = 0;
    if ( cookies.empty() )
    {
        lastId = 1;
    }
    else
    {
        lastId = cookies.rbegin()->first + 1;
    }

    // The new cookie will have now() as current time and that is OK, because
    // in the first read, when the client does not have a cookie id yet, he
    // will receive all data fields anyway, and then, with a cookie, will
    // keep reading only the new ones
    cookies[ lastId ] = newAccessTime;

    return lastId;
}

void Sessioner::clearExpiredCookies( const TimePoint& now )
{
    std::list< int > expiredCookies;

    for ( const auto& [ id, lastAccess ] : cookies )
    {
        if ( now - lastAccess >= inactivityTimeout )
        {
            expiredCookies.push_back( id );
        }
    }

    for ( int id : expiredCookies )
    {
        cookies.erase( id );
    }
}

auto Sessioner::getLastAccessTime( int id, const TimePoint& now ) -> TimePoint
{
    std::lock_guard lock( mutex );

    clearExpiredCookies( now );

    if ( cookies.find( id ) == cookies.end() )
    {
        throw Exception( "Expired!" );
    }

    // Store last access time before modifying the stored time
    const auto lastAccessTime = cookies.at( id );
    // Update stored time, because we have just accessed a cookie
    cookies.at( id ) = now;

    return lastAccessTime;
}

} // namespace rdbus::out::http
