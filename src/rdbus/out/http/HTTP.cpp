#include "HTTP.hpp"
#include "rdbus/config/Output.hpp"
#include <httplib.h>
#include <mutex>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <thread>

using namespace nlohmann;

namespace rdbus::out::http
{

static void exceptionHandler( const httplib::Request& req, httplib::Response& res, std::exception_ptr ep )
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
    res.status = httplib::StatusCode::InternalServerError_500;
}

HTTP::HTTP( const config::Output& settings )
{
    server.Get( "/read",
                [ & ]( const httplib::Request& req, httplib::Response& res )
                {
                    SPDLOG_INFO( "Received HTTP request " + req.path + " from " + req.remote_addr + ":" + std::to_string( req.remote_port ) );

                    json json;
                    // Accessing members, thread unsafe
                    {
                        std::lock_guard lock( mutex );

                        Sessioner::TimePoint lastTime;
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

                        json = buffer.parseFrom( lastTime );
                    }

                    int indent = -1; // no indent by default
                    if ( req.has_param( "pretty" ) )
                    {
                        indent = 4;
                    }
                    res.set_content( json.dump( indent ), "text/plain" );
                } );

    server.set_exception_handler( exceptionHandler );

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
    buffer.add( list );
}

} // namespace rdbus::out::http
