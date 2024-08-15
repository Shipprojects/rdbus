#include "Config.hpp"
#include "exception.hpp"
#include "rdbus/config/Register.hpp"
#include "utility.hpp"
#include <nlohmann/json.hpp>

using namespace nlohmann;

namespace rdbus::config
{

using Slaves = Config::Slaves;

static void checkDuplicateSlaveAddresses( Slaves slaves )
{
    slaves.sort( []( const Slave& left, const Slave& right )
                 { return left.address < right.address; } );

    const auto& it = std::adjacent_find( slaves.begin(), slaves.end(),
                                         []( const Slave& left, const Slave& right )
                                         { return left.address == right.address; } );

    tools::throwIf( it != slaves.end(), "Duplicate slave addresses found!" );
}

static void checkDuplicateSlaveNames( Slaves slaves )
{
    slaves.sort( []( const Slave& left, const Slave& right )
                 { return left.name < right.name; } );

    const auto& it = std::adjacent_find( slaves.begin(), slaves.end(),
                                         []( const Slave& left, const Slave& right )
                                         { return left.name == right.name; } );

    tools::throwIf( it != slaves.end(), "Duplicate slave names found!" );
}

static void checkDuplicateRegisterNames( Slaves slaves )
{
    for ( auto& slave : slaves )
    {
        slave.registers.sort( []( const Register& left, const Register& right )
                              { return left.name < right.name; } );

        const auto& it = std::adjacent_find( slave.registers.begin(), slave.registers.end(),
                                             []( const Register& left, const Register& right )
                                             { return left.name == right.name; } );

        tools::throwIf( it != slave.registers.end(), "Duplicate register names found in single slave!" );
    }
}

static void checkRegisterAddressSpacing( Slaves slaves )
{
    for ( auto& slave : slaves )
    {
        slave.registers.sort( []( const Register& left, const Register& right )
                              { return left.address < right.address; } );

        const auto& it = std::adjacent_find( slave.registers.begin(), slave.registers.end(),
                                             []( const Register& left, const Register& right )
                                             { return left.address + left.byteOrder.size() - 1 >= right.address; } );

        tools::throwIf( it != slave.registers.end(), "Adjacent registers found where addresses overlap each other!" );
    }
}

void from_json( const nlohmann::json& j, Config& x )
{
    std::string protocol;
    tools::parseKeyValue( j, "protocol", protocol, "No protocol name present!" );

    Output output;
    tools::parseKeyValue( j, "output", output, "No output section present!" );

    Serial serial;
    tools::parseKeyValue( j, "serial", serial, "No serial section present!" );

    Slaves slaves;
    tools::parseKeyValue( j, "slaves", slaves, "No slaves present!" );

    if ( slaves.empty() )
    {
        throw ParseException( "No slaves present!" );
    }

    const std::list< std::string > availableProtocols = { "modbus", "nmea" };
    if ( std::find( availableProtocols.begin(), availableProtocols.end(), protocol ) == availableProtocols.end() )
    {
        throw ParseException( "Unsupported protocol selected!" );
    }

    checkRegisterAddressSpacing( slaves );
    checkDuplicateRegisterNames( slaves );
    checkDuplicateSlaveAddresses( slaves );
    checkDuplicateSlaveNames( slaves );

    x.output = output;
    x.protocol = protocol;
    x.serial = serial;
    x.slaves = slaves;
}

} // namespace rdbus::config