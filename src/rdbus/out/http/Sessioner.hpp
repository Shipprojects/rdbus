#pragma once

#include <chrono>
#include <map>
#include <mutex>
#include <stdexcept>

namespace rdbus::out::http
{

// Manages HTTP sessions by storing a cookie of last access time. Expires after a while.
class Sessioner
{
public:
    using TimePoint = std::chrono::time_point< std::chrono::system_clock >;

    // Creates a new session and returns session id
    int create( const TimePoint& newAccessTime = std::chrono::system_clock::now() );

    // Clears expired cookies. Updates last access time for given id. Returns last access time.
    TimePoint getLastAccessTime( int id, const TimePoint& newAccessTime = std::chrono::system_clock::now() );

    struct Exception : public std::runtime_error
    {
        Exception( const std::string& what );
    };

    static constexpr auto inactivityTimeout = std::chrono::minutes( 5 );

private:
    // Map of cookies (ids) and last access times
    std::map< int, TimePoint > cookies;
    std::mutex mutex;

    void clearExpiredCookies( const TimePoint& now );
};

}; // namespace rdbus::out::http