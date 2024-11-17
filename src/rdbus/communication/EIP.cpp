#include "EIP.hpp"
#include <MessageRouter.h>
#include <cip/EPath.h>
#include <spdlog/spdlog.h>
#include <utils/Buffer.h>

using eipScanner::MessageRouter;
using namespace eipScanner::utils;
using namespace eipScanner::cip;

namespace rdbus::communication
{

EIP::EIP( const config::Address& address )
: session( std::make_shared< eipScanner::SessionInfo >( address.ip, address.port ) )
{
}

int16_t EIP::getSingle( const eipScanner::cip::EPath& epath, CipUsint serviceId )
{
    try
    {
        const auto& response = MessageRouter().sendRequest( session, serviceId, epath );
        SPDLOG_INFO( "Sending request " + epath.toString() );

        const auto status = response.getGeneralStatusCode();
        if ( status == GeneralStatusCodes::SUCCESS )
        {
            CipInt result;
            Buffer( response.getData() ) >> result;

            SPDLOG_INFO( "Data received 0x" + ( std::stringstream() << std::hex << result ).str() );

            return result;
        }
        else
        {
            throw Exception( "Response error code - " + std::to_string( status ) );
        }
    }
    catch ( std::exception& e )
    {
        throw Exception( e.what() );
    }
}

EIP::Exception::Exception( const std::string& what )
: rdbus::Exception( "EIP exception - " + what )
{
}

} // namespace rdbus::communication