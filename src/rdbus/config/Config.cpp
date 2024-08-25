#include "Config.hpp"
#include "Exception.hpp"
#include "modbus/Modbus.hpp"
#include "modbus/Register.hpp"
#include "utility.hpp"
#include <nlohmann/json.hpp>

using namespace nlohmann;
using namespace rdbus::config::modbus;
using namespace rdbus::config::nmea;

namespace rdbus::config
{

using Slaves = Modbus::Slaves;
using Registers = Slave::Registers;
using Sentences = NMEA::Sentences;

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
    std::string talkerId;
    tools::parseKeyValue( j, "talker_id", talkerId, "No 'talker_id' field present!" );

    bool withChecksum = false;
    tools::parseKeyValue( j, "checksum", withChecksum, "No 'checksum' field present!" );

    nmea::NMEA::Sentences sentences;
    tools::parseKeyValue( j, "sentences", sentences, "No 'sentences' section present!" );

    checkDuplicateSentenceIDs( sentences );

    x.nmea.sentences = sentences;
    x.nmea.talkerId = talkerId;
    x.nmea.withChecksum = withChecksum;
}

void from_json( const nlohmann::json& j, Config& x )
{
    std::string protocol;
    tools::parseKeyValue( j, "protocol", protocol, "No 'protocol' name present!" );

    Serial serial;
    tools::parseKeyValue( j, "serial", serial, "No 'serial' section present!" );

    if ( protocol == "modbus" )
    {
        parseModbus( j, x );
    }
    else if ( protocol == "nmea" )
    {
        parseNMEA( j, x );
    }
    else
    {
        throw ParseException( "Unsupported protocol " + protocol + "!" );
    }

    x.protocol = protocol;
    x.serial = serial;
}

} // namespace rdbus::config