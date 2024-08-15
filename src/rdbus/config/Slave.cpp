#include "Slave.hpp"
#include "exception.hpp"
#include "utility.hpp"

using namespace nlohmann;

namespace rdbus::config
{

void from_json( const nlohmann::json& j, Slave& x )
{
    std::string name;
    tools::parseKeyValue( j, "name", name, "No slave name present!" );

    int address = 0;
    tools::parseKeyValue( j, "address", address, "No slave address present!" );

    int pollTimeMs = 0;
    tools::parseKeyValue( j, "poll_time_ms", pollTimeMs, "No slave poll time present!" );

    Slave::Registers registers;
    tools::parseKeyValue( j, "registers", registers, "No slave registers present!" );

    if ( registers.empty() )
    {
        throw ParseException( "No slave registers present!" );
    }

    x.name = name;
    x.address = address;
    x.pollTimeMs = Slave::Millis( pollTimeMs );
    x.registers = registers;
}

} // namespace rdbus::config