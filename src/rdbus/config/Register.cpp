#include "Register.hpp"
#include "_utility.hpp"

using namespace nlohmann;

namespace config
{

static std::list< int > parseOrder( const std::string& order )
{
    std::list< int > result;

    for ( const char c : order )
    {
        // Since valid characters are big letters, and
        // we start from 'A', subtract it's value
        result.emplace_back( c - 'A' );
    }

    return result;
}

void from_json( const nlohmann::json& j, Register& x )
{
    std::string name;
    parseKeyValue( j, "name", name );

    int address = 0;
    parseKeyValue( j, "address", address, "No address field present in 'register' section!" );

    std::string order;
    parseKeyValue( j, "data_order", order, "No data_order field present in 'register' section!" );

    x.byteOrder = parseOrder( order );
    x.name = name;
    x.address = address;
}

} // namespace config