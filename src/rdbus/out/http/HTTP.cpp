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

HTTP::HTTP( const config::Output& settings )
{
    server.Get( "/read",
                [ & ]( const httplib::Request& req, httplib::Response& res )
                {
                    SPDLOG_INFO( "Received HTTP request " + req.path + " from " + req.remote_addr + ":" + std::to_string( req.remote_port ) );

                    const auto& json = parse();

                    int indent = -1; // no indent by default
                    if ( req.has_param( "pretty" ) )
                    {
                        indent = 4;
                    }
                    res.set_content( json.dump( indent ), "text/plain" );
                } );

    serverThread = std::thread( std::bind( &httplib::Server::listen,
                                           &server,
                                           settings.ip.value(),
                                           settings.port.value(), 0 ) );
}

HTTP::~HTTP()
{
    server.stop();
    serverThread.join();
}

void HTTP::send( const std::list< rdbus::Data >& list )
{
    std::lock_guard lock( mutex );

    buffer.insert( buffer.end(), list.begin(), list.end() );

    sizeConstrainBuffer();
}

void HTTP::sizeConstrainBuffer()
{
    // Pop back while there is more than 500KB stored in buffer
    constexpr auto maxBufferSize = 500'000;
    while ( buffer.size() * sizeof( rdbus::Data ) > maxBufferSize )
    {
        buffer.pop_back();
    }
}

json HTTP::parse()
{
    std::lock_guard lock( mutex );

    const json data = buffer;
    buffer.clear();
    return data;
}

} // namespace rdbus::out::http
