#include "rdbus/Data.hpp"
#include "rdbus/communication/Communicator.hpp"
#include "rdbus/communication/modbus/Communicator.hpp"
#include "rdbus/config/Config.hpp"
#include "rdbus/out/pipe/Pipe.hpp"
#include <fstream>

int main( int, char** )
{
    std::ifstream jsonFile( "config.json" );
    if ( !jsonFile.good() )
    {
        return 1;
    }

    const rdbus::config::Config config = nlohmann::json::parse( jsonFile );

    rdbus::communication::modbus::Communicator communicator( config.serial );

    rdbus::communication::Communicator& com = communicator;

    std::list< rdbus::Data > list;
    for ( const auto& slave : config.slaves )
    {
        auto data = com.request( slave );
        list.emplace_back( std::move( data ) );
    }

    rdbus::out::pipe::Pipe pipe;
    pipe.send( list );

    return 0;
}
