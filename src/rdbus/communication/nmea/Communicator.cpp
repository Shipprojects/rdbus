#include "Communicator.hpp"
#include "Interpreter.hpp"
#include "Response.hpp"
#include "rdbus/Data.hpp"

namespace rdbus::communication::nmea
{

Communicator::Communicator( const config::Serial& settings, std::unique_ptr< OS > os )
: connection( settings, std::move( os ) )
{
}

std::list< rdbus::Data > Communicator::receive( const NMEA& settings )
{
    std::list< rdbus::Data > data;

    // NMEA may return all data in one single response
    std::vector< uint8_t > rawed;
    try
    {
        rawed = connection.getData();
    }
    catch ( const OS::Timeout& e )
    {
        // It is OK if timeout occurs. Means that talker did not talk. We cannot
        // do anything in this case but wait for next time it speaks
        SPDLOG_WARN( std::string( "NMEA - " ) + e.what() );
    }
    catch ( const OS::Exception& e )
    {
        SPDLOG_ERROR( e.what() );
        data.emplace_back( rdbus::Data{ .deviceName = settings.name } );
        data.back().error = rdbus::Data::Error{ .code = rdbus::Data::Error::OS,
                                                .what = e.what() };
    }

    const auto& timestamp = std::chrono::system_clock::now();
    const auto& chunks = interpreter::split( rawed );
    for ( const auto& chunk : chunks )
    {
        try
        {
            const Response response( chunk, settings.withChecksum );

            // Parse response to data fields
            auto fields = interpreter::parse( response, settings.sentences, timestamp );
            data.emplace_back( rdbus::Data{ .deviceName = settings.name,
                                            .fields = std::move( fields ),
                                            .metadata = response.getSentenceID() } );
        }
        catch ( const Response::Exception& e )
        {
            SPDLOG_ERROR( e.what() );
            data.emplace_back( rdbus::Data{ .deviceName = settings.name } );
            data.back().error = rdbus::Data::Error{ .code = rdbus::Data::Error::NMEA,
                                                    .what = e.what() };
        }
        catch ( const communication::interpreter::Exception& e )
        {
            SPDLOG_ERROR( e.what() );
            data.emplace_back( rdbus::Data{ .deviceName = settings.name } );
            data.back().error = rdbus::Data::Error{ .code = rdbus::Data::Error::NMEA,
                                                    .what = e.what() };
        }
    }

    return data;
}

} // namespace rdbus::communication::nmea
