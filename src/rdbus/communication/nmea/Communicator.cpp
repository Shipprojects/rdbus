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

std::optional< rdbus::Data > Communicator::receive( const NMEA& settings )
{
    std::optional< rdbus::Data > data;
    try
    {
        const auto& rawed = connection.getData();
        const Response response( rawed, settings.withChecksum );

        const auto& timestamp = std::chrono::system_clock::now();

        // Parse response to data fields
        auto fields = interpreter::parse( response, settings.sentences, timestamp );
        data = rdbus::Data{ .deviceName = settings.talkerId,
                            .fields = std::move( fields ) };
    }
    catch ( const Response::Exception& e )
    {
        SPDLOG_ERROR( e.what() );
        data = rdbus::Data{ .deviceName = settings.talkerId };
        data->error = rdbus::Data::Error{ .code = rdbus::Data::Error::NMEA,
                                          .what = e.what() };
    }
    catch ( const communication::interpreter::Exception& e )
    {
        SPDLOG_ERROR( e.what() );
        data = rdbus::Data{ .deviceName = settings.talkerId };
        data->error = rdbus::Data::Error{ .code = rdbus::Data::Error::NMEA,
                                          .what = e.what() };
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
        data = rdbus::Data{ .deviceName = settings.talkerId };
        data->error = rdbus::Data::Error{ .code = rdbus::Data::Error::OS,
                                          .what = e.what() };
    }

    return data;
}

} // namespace rdbus::communication::nmea
