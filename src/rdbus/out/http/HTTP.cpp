#include "HTTP.hpp"
#include <httplib.h>
#include <mutex>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <thread>

using namespace nlohmann;

namespace rdbus::out::http
{

static void exceptionHandler( const httplib::Request&, httplib::Response& res, std::exception_ptr ep )
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

static int getIndent( const httplib::Request& req )
{
    if ( req.has_param( "pretty" ) )
    {
        return 4;
    }

    return -1; // no indent by default
}

HTTP::HTTP( const config::Address& address, const Storage& storage )
: storage( storage )
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

                        json = this->storage.get( lastTime );
                    }

                    const int indent = getIndent( req );
                    res.set_content( json.dump( indent ), "text/plain" );
                } );

    server.Get( "/process/limits",
                [ & ]( const httplib::Request& req, httplib::Response& res )
                {
                    SPDLOG_INFO( "Received HTTP request " + req.path + " from " + req.remote_addr + ":" + std::to_string( req.remote_port ) );

                    json json;
                    // Accessing members, thread unsafe
                    {
                        std::lock_guard lock( mutex );
                        json = this->storage.get( Sessioner::TimePoint(), processing::Name::Limits );
                    }

                    const int indent = getIndent( req );
                    res.set_content( json.dump( indent ), "text/plain" );
                } );


    server.set_exception_handler( exceptionHandler );

    serverThread = std::thread( std::bind( &httplib::Server::listen,
                                           &server,
                                           address.ip,
                                           address.port,
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
    storage.put( list );
}

void HTTP::send( const processing::Base::OutputList& list, processing::Name name )
{
    std::lock_guard lock( mutex );
    storage.put( list, name );
}

} // namespace rdbus::out::http
