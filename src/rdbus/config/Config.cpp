#include "Config.hpp"
#include "Exception.hpp"
#include "modbus/Modbus.hpp"
#include "modbus/Register.hpp"
#include "utility.hpp"
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace rdbus::config::modbus;
using namespace rdbus::config::nmea;
using namespace rdbus::config::processors;
using namespace rdbus::config::wago;

namespace rdbus::config
{

using Slaves = Modbus::Slaves;
using Registers = Slave::Registers;
using Sentences = NMEA::Sentences;

static void checkDuplicateModuleNames( std::list< Module > modules )
{
    modules.sort( []( const Module& left, const Module& right )
                  { return left.name < right.name; } );

    const auto& it = std::adjacent_find( modules.begin(), modules.end(),
                                         []( const Module& left, const Module& right )
                                         {
                                             return left.name == right.name;
                                         } );

    tools::throwIf( it != modules.end(), "Duplicate module names found!" );
}

static void validateLimitModules( const std::list< Module >& modules, const Limits& limits )
{
    for ( const auto& device : limits.devices )
    {
        tools::throwIf( std::find_if( modules.begin(), modules.end(), [ & ]( const Module& module )
                                      { return device == module.name; } ) == modules.end(),
                        "No module named " + device + " found in 'modules' section!" );
    }
}

static void setOffsetValues( std::list< Module >& modules )
{
    if ( modules.size() == 1 )
    {
        return;
    }

    for ( auto it = std::next( modules.begin() ); it != modules.end(); it++ )
    {
        // If offset is not set
        if ( it->offset == Module::defaultOffset )
        {
            const auto previous = std::prev( it );
            it->offset = previous->offset + previous->instances.size();
        }
    }
}

static void checkOverlappingOffsets( std::list< Module >& modules )
{
    if ( modules.size() == 1 )
    {
        return;
    }

    for ( auto it = std::next( modules.begin() ); it != modules.end(); it++ )
    {
        // Offset overlapping can only happen if the offset is set manually
        // The offsets, which are set manually, arrive here with non-default value
        if ( it->offset != Module::defaultOffset )
        {
            const auto previous = std::prev( it );
            tools::throwIf( it->offset < ( previous->offset + previous->instances.size() ), "Module offset overlapping detected!" );
        }
    }
}

static void checkDuplicateSentenceIDs( Sentences sentences )
{
    sentences.sort( []( const Sentence& left, const Sentence& right )
                    { return left.id < right.id; } );

    const auto& it = std::adjacent_find( sentences.begin(), sentences.end(),
                                         []( const Sentence& left, const Sentence& right )
                                         {
                                             return left.id == right.id;
                                         } );

    tools::throwIf( it != sentences.end(), "Duplicate sentence IDs found!" );
}

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

static void parseModbus( const nlohmann::json& j, Config& x )
{
    Slaves slaves;
    tools::parseKeyValue( j, "slaves", slaves, "No 'slaves' present!" );

    tools::throwIf( slaves.empty(), "No slaves present!" );

    checkRegisterAddressSpacing( slaves );
    checkDuplicateRegisterNames( slaves );
    checkDuplicateSlaveIDs( slaves );
    checkDuplicateSlaveNames( slaves );

    x.modbus.slaves = slaves;
}

static void parseNMEA( const nlohmann::json& j, Config& x )
{
    std::string name;
    tools::parseKeyValue( j, "name", name, "No 'name' field present!" );

    bool withChecksum = false;
    tools::parseKeyValue( j, "checksum", withChecksum, "No 'checksum' field present!" );

    nmea::NMEA::Sentences sentences;
    tools::parseKeyValue( j, "sentences", sentences, "No 'sentences' section present!" );

    checkDuplicateSentenceIDs( sentences );

    x.nmea.sentences = sentences;
    x.nmea.name = name;
    x.nmea.withChecksum = withChecksum;
}

static void parseWago( const nlohmann::json& j, Config& x )
{
    std::list< Module > modules;
    tools::parseKeyValue( j, "modules", modules, "No 'modules' section present!" );

    // TODO
    // This is a hack! 'data_processors' has to be checked in main from_json function
    // to be able to validate limit modules for all protocols, and not only that of wago.
    // It is currently permissible to do so only because limits module works with wago
    // only.
    if ( j.contains( "data_processors" ) )
    {
        Processors processors;
        tools::parseKeyValue( j, "data_processors", processors );

        if ( processors.limits.has_value() )
        {
            validateLimitModules( modules, *processors.limits );
        }

        x.processors = processors;
    }

    checkDuplicateModuleNames( modules );
    checkOverlappingOffsets( modules );
    setOffsetValues( modules );

    x.wago.modules = modules;
}

void from_json( const nlohmann::json& j, Config& x )
{
    tools::throwIf( j.contains( "serial" ) && j.contains( "address" ), "Sections 'serial' and 'address' are mutually exclusive!" );
    tools::throwIf( !j.contains( "serial" ) && !j.contains( "address" ), "No 'serial' or 'address' sections present!" );

    std::string protocol;
    tools::parseKeyValue( j, "protocol", protocol, "No 'protocol' name present!" );

    if ( j.contains( "serial" ) )
    {
        Serial serial;
        tools::parseKeyValue( j, "serial", serial );
        x.serial = serial;
    }

    if ( j.contains( "address" ) )
    {
        Address address;
        tools::parseKeyValue( j, "address", address );
        x.address = address;
    }

    if ( j.contains( "data_processors" ) )
    {
        Processors processors;
        tools::parseKeyValue( j, "data_processors", processors );

        tools::throwIf( j.contains( "limits" ) && ( protocol == "modbus" || protocol == "nmea" ),
                        "'limits' processor is currently available for wago protocol only!" );
    }

    if ( protocol == "modbus" )
    {
        parseModbus( j, x );
    }
    else if ( protocol == "nmea" )
    {
        parseNMEA( j, x );
    }
    else if ( protocol == "wago" )
    {
        parseWago( j, x );
    }
    else
    {
        throw ParseException( "Unsupported protocol " + protocol + "!" );
    }

    x.protocol = protocol;
}

} // namespace rdbus::config