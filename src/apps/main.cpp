#include "rdbus/communication/Communicator.hpp"
#include "rdbus/communication/modbus/Communicator.hpp"
#include "rdbus/config/Config.hpp"
#include <fstream>

int main( int, char** )
{
    std::ifstream jsonFile( "config.json" );
    if ( !jsonFile.good() )
    {
        return 1;
    }

    const config::Config config = nlohmann::json::parse( jsonFile );

    communication::modbus::Communicator communicator( config.serial );

    communication::Communicator& com = communicator;

    for ( const auto& slave : config.slaves )
    {
        com.request( slave );
    }

    return 0;
}
