#include "Register.hpp"
#include "rdbus/Data.hpp"
#include "rdbus/config/exception.hpp"
#include "utility.hpp"

using namespace nlohmann;
using namespace rdbus;

namespace rdbus::config
{

static void throwIf( bool condition, const std::string& message )
{
    if ( condition )
    {
        throw ParseException( message );
    }
}

static std::pair< Type, std::list< int > > getWildType( const std::string& typeString, const std::string& orderString )
{
    throwIf( orderString.size() < sizeof( uint16_t ), "Data order " + orderString + " cannot be smaller than 2 bytes!" );
    throwIf( orderString.size() > sizeof( uint64_t ), "Data order " + orderString + " cannot be bigger than 8 bytes!" );
    throwIf( orderString.size() % 2, "Data order " + orderString + " does not contain even amount of characters!" );
    // For case when there are 6 characters in data orer
    throwIf( orderString.size() / 2 == 3, "Data order " + orderString + " is not a power of 2!" );
    throwIf( typeString != "F" && typeString != "U" && typeString != "S", "Unknown data type " + typeString + "!" );
    throwIf( typeString == "F" && orderString.size() < sizeof( float ), "Order of float cannot be less than 4 bytes!" );

    std::list< int > order;
    for ( const char c : orderString )
    {
        // Since valid characters are big letters, and
        // we start from 'A', subtract it's value
        order.emplace_back( c - 'A' );
    }

    Type type;
    if ( typeString == "F" )
    {
        switch ( order.size() )
        {
            case 4:
                type = Type::Float;
                break;
            case 8:
                type = Type::Double;
                break;
        }
    }
    else if ( typeString == "U" )
    {
        switch ( order.size() )
        {
            case 2:
                type = Type::Uint16;
                break;
            case 4:
                type = Type::Uint32;
                break;
            case 8:
                type = Type::Uint64;
                break;
        }
    }
    else if ( typeString == "S" )
    {
        switch ( order.size() )
        {
            case 2:
                type = Type::Int16;
                break;
            case 4:
                type = Type::Int32;
                break;
            case 8:
                type = Type::Int64;
                break;
        }
    }

    return { type, order };
}

static std::pair< Type, std::list< int > > getStandardType( const std::string& type )
{
    static std::map< std::string, std::pair< Type, std::list< int > > > typeMap = {

        // 64-bit float (double) --------------------------------
        { "F64A", { Type::Double, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { "F64B", { Type::Double, { 7, 6, 5, 4, 3, 2, 1, 0 } } },

        // 32-bit float
        { "F32A", { Type::Float, { 0, 1, 2, 3 } } },
        { "F32B", { Type::Float, { 1, 0, 3, 2 } } },
        { "F32C", { Type::Float, { 2, 3, 0, 1 } } },
        { "F32D", { Type::Float, { 3, 2, 1, 0 } } },

        // 64-bit unsigned integer ------------------------------
        { "U64A", { Type::Uint64, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { "U64B", { Type::Uint64, { 7, 6, 5, 4, 3, 2, 1, 0 } } },

        // 32-bit unsigned integer
        { "U32A", { Type::Uint32, { 0, 1, 2, 3 } } },
        { "U32B", { Type::Uint32, { 1, 0, 3, 2 } } },
        { "U32C", { Type::Uint32, { 2, 3, 0, 1 } } },
        { "U32D", { Type::Uint32, { 3, 2, 1, 0 } } },

        // 16-bit unsigned integer
        { "U16A", { Type::Uint16, { 0, 1 } } },
        { "U16B", { Type::Uint16, { 1, 0 } } },

        // 64-bit signed integer --------------------------------
        { "S64A", { Type::Uint64, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { "S64B", { Type::Uint64, { 7, 6, 5, 4, 3, 2, 1, 0 } } },

        // 32-bit signed integer
        { "S32A", { Type::Int32, { 0, 1, 2, 3 } } },
        { "S32B", { Type::Int32, { 1, 0, 3, 2 } } },
        { "S32C", { Type::Int32, { 2, 3, 0, 1 } } },
        { "S32D", { Type::Int32, { 3, 2, 1, 0 } } },

        // 16-bit signed integer
        { "S16A", { Type::Int16, { 0, 1 } } },
        { "S16B", { Type::Int16, { 1, 0 } } },
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
    tools::parseKeyValue( j, "name", name, "No name field present in 'register' section!" );

    int address = 0;
    tools::parseKeyValue( j, "address", address, "No address field present in 'register' section!" );

    std::string order;
    tools::parseKeyValue( j, "data_order", order );

    std::string type;
    tools::parseKeyValue( j, "data_type", type, "No 'data_type' field present in 'register' section!" );


    // If wild data type
    if ( type.size() == 1 )
    {
        const auto& [ dataType, dataOrder ] = getWildType( type, order );
        x.type = dataType;
        x.byteOrder = dataOrder;
    }
    else
    {
        const auto& [ dataType, dataOrder ] = getStandardType( type );
        x.type = dataType;
        x.byteOrder = dataOrder;
    }

    x.name = name;
    x.address = address;
}

} // namespace rdbus::config