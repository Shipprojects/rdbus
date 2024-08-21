#include "Sentence.hpp"
#include "rdbus/config/utility.hpp"

namespace rdbus::config::nmea
{

void from_json( const nlohmann::json& j, Sentence& x )
{
    std::string id;
    tools::parseKeyValue( j, "id", id, "No 'id' field present in 'sentence' section!" );

    Sentence::Fields fields;
    tools::parseKeyValue( j, "fields", fields, "No 'fields' present in 'sentence' section!" );

    tools::throwIf( fields.size() == 0, "Empty 'fields' in 'sentence' section!" );

    x.id = id;
    x.fields = fields;
}

} // namespace rdbus::config::nmea
