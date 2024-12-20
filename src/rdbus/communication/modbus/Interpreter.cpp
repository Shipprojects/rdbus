#include "Interpreter.hpp"
#include "rdbus/Data.hpp"
#include "rdbus/communication/Interpreter.hpp"
#include <cstdint>

namespace rdbus::communication::modbus::interpreter
{

namespace tools
{

Exception::Exception( const std::string& what )
: rdbus::communication::interpreter::Exception( "Modbus - " + what )
{
}

RawUint16List toRaw16BitRegisters( const std::vector< MB::ModbusCell >& input )
{
    RawUint16List output;

    for ( const auto& cell : input )
    {
        const uint16_t reg = cell.reg();
        const uint8_t lsb = reg & 0xff;
        const uint8_t msb = ( reg >> 8 ) & 0xff;
        // Swap byte order, because Modbus library does it too when storing register values,
        // so we actually result with the same order as data before parsing through the library
        output.push_back( { msb, lsb } );
    }

    return output;
}

RawMergedList toRawMergedRegisters( const RawUint16List& input, const Registers& registers )
{
    RawMergedList output;

    int i = 0;
    for ( const auto& reg : registers )
    {
        std::vector< uint8_t > word;

        const auto end = ( reg.byteOrder.size() / sizeof( uint16_t ) ) + i;
        for ( ; i < end; i++ )
        {
            word.insert( word.end(), input[ i ].begin(), input[ i ].end() );
        }

        output.emplace_back( std::move( word ) );
    }

    return output;
}

BigEndianRegisters toUserInterpretation( const RawMergedList& input, const Registers& registers )
{
    if ( input.size() != registers.size() )
    {
        throw Exception( "Config register and merged input register count mismatch!" );
    }

    BigEndianRegisters output;

    auto regIt = registers.begin();
    auto inputIt = input.begin();
    for ( ; regIt != registers.end() && inputIt != input.end(); regIt++, inputIt++ )
    {
        std::vector< uint8_t > word;

        for ( const auto position : regIt->byteOrder )
        {
            word.push_back( ( *inputIt )[ position ] );
        }

        output.emplace_back( std::move( word ) );
    }

    return output;
}

SmallEndianRegisters toMachineInterpretation( const SmallEndianRegisters& input )
{
    SmallEndianRegisters output;

    for ( auto word : input )
    {
        std::reverse( word.begin(), word.end() );
        output.emplace_back( std::move( word ) );
    }

    return output;
}

Fields toParsedFields( const SmallEndianRegisters& input, const Registers& registers, const Timestamp& timestamp )
{
    if ( input.size() != registers.size() )
    {
        throw Exception( "Config register and input register count mismatch!" );
    }

    using namespace rdbus;

    Fields output;

    auto regIt = registers.begin();
    auto inputIt = input.begin();
    for ( ; regIt != registers.end() && inputIt != input.end(); regIt++, inputIt++ )
    {
        const int i = output.size();
        rdbus::Data::Field field = {
            .name = regIt->name,
            .timestamp = timestamp
        };

        switch ( regIt->type )
        {
            case Type::Int16:
                field.value = *reinterpret_cast< const int16_t* >( inputIt->data() );
                break;
            case Type::Uint16:
                field.value = *reinterpret_cast< const uint16_t* >( inputIt->data() );
                break;
            case Type::Int32:
                field.value = *reinterpret_cast< const int32_t* >( inputIt->data() );
                break;
            case Type::Uint32:
                field.value = *reinterpret_cast< const uint32_t* >( inputIt->data() );
                break;
            case Type::Int64:
                field.value = *reinterpret_cast< const int64_t* >( inputIt->data() );
                break;
            case Type::Uint64:
                field.value = *reinterpret_cast< const uint64_t* >( inputIt->data() );
                break;
            case Type::Float:
                field.value = *reinterpret_cast< const float* >( inputIt->data() );
                break;
            case Type::Double:
                field.value = *reinterpret_cast< const double* >( inputIt->data() );
                break;
            default:
                throw Exception( "Unsupported type " + std::to_string( static_cast< int >( regIt->type ) ) + "!" );
                break;
        }

        output.emplace_back( std::move( field ) );
    }

    return output;
}

} // namespace tools

std::list< rdbus::Data::Field > parse( const MB::ModbusResponse& response,
                                       const std::list< config::modbus::Register >& registers,
                                       const rdbus::Data::Field::Timestamp& timestamp )
{
    using namespace tools;

    const auto& rawUint16List = toRaw16BitRegisters( response.registerValues() );
    const auto& rawMergedList = toRawMergedRegisters( rawUint16List, registers );
    const auto& bigEndianRegisters = toUserInterpretation( rawMergedList, registers );
    const auto& littleEndianRegisters = toMachineInterpretation( bigEndianRegisters );
    const auto& fields = toParsedFields( littleEndianRegisters, registers, timestamp );
    return fields;
}

} // namespace rdbus::communication::modbus::interpreter