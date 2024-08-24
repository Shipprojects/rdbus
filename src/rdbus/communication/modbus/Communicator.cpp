#include "Communicator.hpp"
#include "Interpreter.hpp"
#include "MB/modbusException.hpp"
#include "RequestPlanner.hpp"
#include "rdbus/Data.hpp"

namespace rdbus::communication::modbus
{

Communicator::Communicator( const config::Serial& settings, std::unique_ptr< OS > os )
: adapter( settings, std::move( os ) )
{
}

rdbus::Data Communicator::request( const config::modbus::Slave& slave )
{
    // Contains planned list of requests and a separate list of registers which
    // match the order of requests
    const auto& requestDescriptions = requestPlan( slave );

    rdbus::Data data;
    data.deviceName = slave.name;
    try
    {
        for ( const auto& description : requestDescriptions )
        {
            const auto& response = adapter.send( description.request );

            const auto& timestamp = std::chrono::system_clock::now();
            // Parse response to data fields
            auto fields = interpreter::parse( response, description.registers, timestamp );
            for ( auto& field : fields )
            {
                data.fields.emplace_back( std::move( field ) );
            }
        }
    }
    catch ( const MB::ModbusException& e )
    {
        SPDLOG_ERROR( e.what() );
        data.fields.clear();
        data.error = rdbus::Data::Error{ .code = rdbus::Data::Error::Modbus,
                                         .what = e.what() };
    }
    catch ( const OS::Exception& e )
    {
        SPDLOG_ERROR( e.what() );
        data.fields.clear();
        data.error = rdbus::Data::Error{ .code = rdbus::Data::Error::OS,
                                         .what = e.what() };
    }

    return data;
}

} // namespace rdbus::communication::modbus
