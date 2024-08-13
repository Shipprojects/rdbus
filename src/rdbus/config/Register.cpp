#include "Register.hpp"
#include "_utility.hpp"
#include "rdbus/Data.hpp"
#include "rdbus/config/exception.hpp"

using namespace nlohmann;
using namespace rdbus;

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

static std::pair< Type, std::list< int > > getStandardType( const std::string& type )
{
    static std::map< std::string, std::pair< Type, std::list< int > > > typeMap = {
        { "F32A", { Type::Float, { 0, 1, 2, 3 } } },
        { "F32B", { Type::Float, { 1, 0, 3, 2 } } },
        { "F32C", { Type::Float, { 2, 3, 0, 1 } } },
        { "F32D", { Type::Float, { 3, 2, 1, 0 } } },
        { "U32A", { Type::Uint32, { 0, 1, 2, 3 } } },
        { "U32B", { Type::Uint32, { 1, 0, 3, 2 } } },
        { "U32C", { Type::Uint32, { 2, 3, 0, 1 } } },
        { "U32D", { Type::Uint32, { 3, 2, 1, 0 } } },
        { "S32A", { Type::Int32, { 0, 1, 2, 3 } } },
        { "S32B", { Type::Int32, { 1, 0, 3, 2 } } },
        { "S32C", { Type::Int32, { 2, 3, 0, 1 } } },
        { "S32D", { Type::Int32, { 3, 2, 1, 0 } } },
    };

    const auto& it = typeMap.find( type );
    if ( it == typeMap.end() )
    {
        throw ParseException( "Unknown data type " + type + "!" );
    }

    return it->second;
}

void from_json( const nlohmann::json& j, Register& x )
{
    std::string name;
    parseKeyValue( j, "name", name, "No name field present in 'register' section!" );

    int address = 0;
    parseKeyValue( j, "address", address, "No address field present in 'register' section!" );

    std::string order;
    parseKeyValue( j, "data_order", order );

    std::string type;
    parseKeyValue( j, "data_type", type );

    if ( order.empty() && type.empty() )
    {
        throw ParseException( "No 'data_type' or 'data_order' fields present in 'register' section!" );
    }

    if ( !type.empty() )
    {
        const auto& [ dataType, dataOrder ] = getStandardType( type );
        x.type = dataType;
        x.byteOrder = dataOrder;
    }
    else if ( !order.empty() )
    {
        x.byteOrder = parseOrder( order );
        x.type = rdbus::Type::Blob;
    }

    x.name = name;
    x.address = address;
}

} // namespace config