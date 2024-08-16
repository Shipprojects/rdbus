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

    int id = 0;
    tools::parseKeyValue( j, "id", id, "No slave id present!" );

    int pollTimeMs = 0;
    tools::parseKeyValue( j, "poll_time_ms", pollTimeMs, "No slave poll time present!" );

    Slave::Registers registers;
    tools::parseKeyValue( j, "registers", registers, "No slave registers present!" );

    if ( registers.empty() )
    {
        throw ParseException( "No slave registers present!" );
    }

    x.name = name;
    x.id = id;
    x.pollTimeMs = Slave::Millis( pollTimeMs );
    x.registers = registers;
}

} // namespace rdbus::config