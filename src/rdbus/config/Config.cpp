#include "Config.hpp"
#include "exception.hpp"
#include "modbus/Register.hpp"
#include "utility.hpp"
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace rdbus::config::modbus;

namespace rdbus::config
{

using Slaves = Config::Slaves;
using Registers = modbus::Slave::Registers;

static void checkDuplicateSlaveIDs( Slaves slaves )
{
    slaves.sort( []( const Slave& left, const Slave& right )
                 { return left.id < right.id; } );

    const auto& it = std::adjacent_find( slaves.begin(), slaves.end(),
                                         []( const Slave& left, const Slave& right )
                                         {
                                             return left.id == right.id;
                                         } );

    tools::throwIf( it != slaves.end(), "Duplicate slave IDs found!" );
}

static void checkDuplicateSlaveNames( Slaves slaves )
{
    slaves.sort( []( const Slave& left, const Slave& right )
                 { return left.name < right.name; } );

    const auto& it = std::adjacent_find( slaves.begin(), slaves.end(),
                                         []( const Slave& left, const Slave& right )
                                         {
                                             return left.name == right.name;
                                         } );

    tools::throwIf( it != slaves.end(), "Duplicate slave names found!" );
}

static void checkDuplicateRegisterNames( Slaves slaves )
{
    for ( auto& slave : slaves )
    {
        Slave::Registers registers;
        registers.insert( registers.end(), slave.inputRegisters.begin(), slave.inputRegisters.end() );
        registers.insert( registers.end(), slave.outputRegisters.begin(), slave.outputRegisters.end() );

        registers.sort( []( const Register& left, const Register& right )
                        { return left.name < right.name; } );

        const auto& it = std::adjacent_find( registers.begin(), registers.end(),
                                             []( const Register& left, const Register& right )
                                             {
                                                 return left.name == right.name;
                                             } );

        tools::throwIf( it != registers.end(), "Duplicate register names found in single slave!" );
    }
}

static void checkRegisterAddressSpacing( Slaves slaves )
{
    const auto checker = []( Registers& registers )
    {
        registers.sort( []( const Register& left, const Register& right )
                        { return left.address < right.address; } );

        const auto& it = std::adjacent_find( registers.begin(), registers.end(),
                                             []( const Register& left, const Register& right )
                                             {
                                                 // e.g. 3(address) + 4(register byte size)/2(modbus word size) > 4 (next address in config)
                                                 return left.address + left.byteOrder.size() / sizeof( uint16_t ) > right.address;
                                             } );

        tools::throwIf( it != registers.end(), "Adjacent registers found where addresses overlap each other!" );
    };

    for ( auto& slave : slaves )
    {
        checker( slave.inputRegisters );
        checker( slave.outputRegisters );
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
    checkDuplicateSlaveIDs( slaves );
    checkDuplicateSlaveNames( slaves );

    x.output = output;
    x.protocol = protocol;
    x.serial = serial;
    x.slaves = slaves;
}

} // namespace rdbus::config