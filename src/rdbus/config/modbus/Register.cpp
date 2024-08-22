#include "Register.hpp"
#include "rdbus/Data.hpp"
#include "rdbus/config/exception.hpp"
#include "rdbus/config/utility.hpp"

using namespace nlohmann;
using namespace rdbus;
using namespace rdbus::config::tools;

namespace rdbus::config::modbus
{

static std::pair< Type, std::list< int > > getWildType( const std::string& typeString, const std::string& orderString )
{
    throwIf( orderString.size() < sizeof( uint16_t ), "Data order " + orderString + " cannot be smaller than 2 bytes!" );
    throwIf( orderString.size() > sizeof( uint64_t ), "Data order " + orderString + " cannot be bigger than 8 bytes!" );
    throwIf( orderString.size() % 2, "Data order " + orderString + " does not contain even amount of characters!" );
    // For case when there are 6 characters in data orer
    throwIf( orderString.size() / 2 == 3, "Data order " + orderString + " is not a power of 2!" );
    throwIf( typeString != "FLOAT" && typeString != "UINT" && typeString != "INT", "Unknown data type " + typeString + "!" );
    throwIf( typeString == "FLOAT" && orderString.size() < sizeof( float ), "Order of float cannot be less than 4 bytes!" );

    std::list< int > order;
    for ( const char c : orderString )
    {
        // Since valid characters are big letters, and
        // we start from 'A', subtract it's value
        order.emplace_back( c - 'A' );
    }

    Type type;
    if ( typeString == "FLOAT" )
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
    else if ( typeString == "UINT" )
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
    else if ( typeString == "INT" )
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
    // All those various data types with user defined sizes are required in Modbus part
    // of rdbus, because we later rely on those sizes to read correct amount of bytes
    // into fields.
    static std::map< std::string, std::pair< Type, std::list< int > > > typeMap = {

        // 64-bit float (double) --------------------------------
        { "FLOAT_64A", { Type::Double, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { "FLOAT_64B", { Type::Double, { 7, 6, 5, 4, 3, 2, 1, 0 } } },

        // 32-bit float
        { "FLOAT_32A", { Type::Float, { 0, 1, 2, 3 } } },
        { "FLOAT_32B", { Type::Float, { 1, 0, 3, 2 } } },
        { "FLOAT_32C", { Type::Float, { 2, 3, 0, 1 } } },
        { "FLOAT_32D", { Type::Float, { 3, 2, 1, 0 } } },

        // 64-bit unsigned integer ------------------------------
        { "UINT_64A", { Type::Uint64, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { "UINT_64B", { Type::Uint64, { 7, 6, 5, 4, 3, 2, 1, 0 } } },

        // 32-bit unsigned integer
        { "UINT_32A", { Type::Uint32, { 0, 1, 2, 3 } } },
        { "UINT_32B", { Type::Uint32, { 1, 0, 3, 2 } } },
        { "UINT_32C", { Type::Uint32, { 2, 3, 0, 1 } } },
        { "UINT_32D", { Type::Uint32, { 3, 2, 1, 0 } } },

        // 16-bit unsigned integer
        { "UINT_16A", { Type::Uint16, { 0, 1 } } },
        { "UINT_16B", { Type::Uint16, { 1, 0 } } },

        // 64-bit signed integer --------------------------------
        { "INT_64A", { Type::Int64, { 0, 1, 2, 3, 4, 5, 6, 7 } } },
        { "INT_64B", { Type::Int64, { 7, 6, 5, 4, 3, 2, 1, 0 } } },

        // 32-bit signed integer
        { "INT_32A", { Type::Int32, { 0, 1, 2, 3 } } },
        { "INT_32B", { Type::Int32, { 1, 0, 3, 2 } } },
        { "INT_32C", { Type::Int32, { 2, 3, 0, 1 } } },
        { "INT_32D", { Type::Int32, { 3, 2, 1, 0 } } },

        // 16-bit signed integer
        { "INT_16A", { Type::Int16, { 0, 1 } } },
        { "INT_16B", { Type::Int16, { 1, 0 } } },
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
    if ( type == "FLOAT" || type == "UINT" || type == "INT" )
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

} // namespace rdbus::config::modbus