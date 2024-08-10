#include "Slave.hpp"
#include "_utility.hpp"
#include "exception.hpp"

using namespace nlohmann;

namespace config
{

void from_json( const nlohmann::json& j, Slave& x )
{
    std::string name;
    parseKeyValue( j, "name", name, "No slave name present!" );

    int address = 0;
    parseKeyValue( j, "address", address, "No slave address present!" );

    int pollTimeMs = 0;
    parseKeyValue( j, "poll_time_ms", pollTimeMs, "No slave poll time present!" );

    Slave::Registers registers;
    parseKeyValue( j, "registers", registers, "No slave registers present!" );

    bool CRC = false;
    parseKeyValue( j, "CRC", CRC, "No CRC field present in slave settings!" );

    if ( registers.empty() )
    {
        throw ParseException( "No slave registers present!" );
    }

    x.name = name;
    x.CRC = CRC;
    x.address = address;
    x.pollTimeMs = Slave::Millis( pollTimeMs );
    x.registers = registers;
}

} // namespace config