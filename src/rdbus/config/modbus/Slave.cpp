#include "Slave.hpp"
#include "rdbus/config/utility.hpp"

using namespace nlohmann;

namespace rdbus::config::modbus
{

void from_json( const nlohmann::json& j, Slave& x )
{
    std::string name;
    tools::parseKeyValue( j, "name", name, "No slave 'name' present!" );

    int id = 0;
    tools::parseKeyValue( j, "id", id, "No slave 'id' present!" );

    int pollTimeMs = 0;
    tools::parseKeyValue( j, "poll_time_ms", pollTimeMs, "No slave 'poll_time_ms' present!" );

    Slave::Registers inputRegisters;
    tools::parseKeyValue( j, "analog_input_registers", inputRegisters );

    Slave::Registers outputRegisters;
    tools::parseKeyValue( j, "analog_output_registers", outputRegisters );

    tools::throwIf( inputRegisters.empty() && outputRegisters.empty(), "No slave 'analog_input_registers' or 'analog_output_registers'  present!" );

    x.name = name;
    x.id = id;
    x.pollTimeMs = Slave::Millis( pollTimeMs );
    x.inputRegisters = inputRegisters;
    x.outputRegisters = outputRegisters;
}

} // namespace rdbus::config::modbus