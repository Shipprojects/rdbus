#include "HTTP.hpp"
#include "rdbus/config/Output.hpp"
#include <algorithm>
#include <chrono>
#include <httplib.h>
#include <mutex>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <thread>

using namespace nlohmann;

namespace rdbus::out::http
{

HTTP::HTTP( const config::Output& settings )
{
    server.Get( "/read",
                [ & ]( const httplib::Request& req, httplib::Response& res )
                {
                    SPDLOG_INFO( "Received HTTP request " + req.path + " from " + req.remote_addr + ":" + std::to_string( req.remote_port ) );

                    TimePoint lastTime;

                    if ( req.has_header( "Cookie" ) )
                    {
                        const auto& sessionString = req.get_header_value( "Cookie" );
                        const int sessionId = std::atoi( sessionString.substr( sessionString.find( '=' ) + 1 ).c_str() );
                        lastTime = sessioner.getLastAccessTime( sessionId );
                    }
                    else
                    {
                        const int sessionId = sessioner.create();
                        res.set_header( "Set-Cookie", std::to_string( sessionId ) );
                    }

                    const auto& json = parseFrom( lastTime );

                    int indent = -1; // no indent by default
                    if ( req.has_param( "pretty" ) )
                    {
                        indent = 4;
                    }
                    res.set_content( json.dump( indent ), "text/plain" );
                } );

    server.set_exception_handler(
    []( const auto& req, auto& res, std::exception_ptr ep )
    {
        std::string errorMessage;
        try
        {
            std::rethrow_exception( ep );
        }
        catch ( const Sessioner::Exception& e )
        {
            errorMessage = e.what();
            SPDLOG_ERROR( errorMessage );
            res.status = httplib::StatusCode::Unauthorized_401;
        }
        catch ( const std::exception& e )
        {
            errorMessage = "HTTP - " + std::string( e.what() );
            SPDLOG_ERROR( errorMessage );
        }
        catch ( ... )
        {
            errorMessage = "HTTP - Unknown exception cought!";
            SPDLOG_ERROR( errorMessage );
        }
        res.set_content( errorMessage, "text/plain" );
        res.status = httplib::StatusCode::InternalServerError_500; } );

    serverThread = std::thread( std::bind( &httplib::Server::listen,
                                           &server,
                                           settings.ip.value(),
                                           settings.port.value(),
                                           0 ) );
}

HTTP::~HTTP()
{
    server.stop();
    serverThread.join();
}

void HTTP::send( const std::list< rdbus::Data >& list )
{
    std::lock_guard lock( mutex );

    for ( const auto& entry : list )
    {
        buffer.emplace_back( std::chrono::system_clock::now(), entry );
    }

    sizeConstrainBuffer();
}

void HTTP::sizeConstrainBuffer()
{
    // Pop front of the buffer while there is more than 500KB stored there
    constexpr auto maxBufferSize = 500'000;
    while ( buffer.size() * sizeof( rdbus::Data ) > maxBufferSize )
    {
        buffer.pop_front();
    }
}

json HTTP::parseFrom( const TimePoint& lastTime )
{
    std::lock_guard lock( mutex );

    // Find the data entry whos timepoint is bigger than the passed lastTime, because
    // the bigger the timepoint the later it came.
    // Note that the buffer will always be sorted relative according to the timepoints
    // of fields.
    const auto& begin = std::upper_bound( buffer.begin(), buffer.end(), lastTime,
                                          []( const TimePoint& value, const TimeDataPairs::value_type& pair )
                                          {
                                              return value < pair.first;
                                          } );

    auto data = json::array();
    std::for_each( begin, buffer.end(),
                   [ & ]( const TimeDataPairs::value_type& pair )
                   {
                       data.push_back( pair.second );
                   } );


    return data;
}

} // namespace rdbus::out::http
