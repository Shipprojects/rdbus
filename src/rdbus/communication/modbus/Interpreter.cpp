#include "Interpreter.hpp"
#include "rdbus/Data.hpp"
#include <cstdint>
#include <stdexcept>

namespace rdbus::communication::modbus::interpreter
{

namespace tools
{

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

    assert( output.size() == registers.size() );

    return output;
}

BigEndianRegisters toUserInterpretation( const RawMergedList& input, const Registers& registers )
{
    BigEndianRegisters output;

    for ( const auto& reg : registers )
    {
        const int i = output.size();
        std::vector< uint8_t > word;

        for ( const auto position : reg.byteOrder )
        {
            word.push_back( input[ i ][ position ] );
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
    assert( input.size() == registers.size() );

    using namespace rdbus;

    Fields output;

    for ( const auto& reg : registers )
    {
        const int i = output.size();
        rdbus::Data::Field field = {
            .name = reg.name,
            .type = reg.type,
            .timestamp = timestamp
        };

        switch ( reg.type )
        {
            case Type::Int16:
                field.value = *reinterpret_cast< const int16_t* >( input[ i ].data() );
                break;
            case Type::Uint16:
                field.value = *reinterpret_cast< const uint16_t* >( input[ i ].data() );
                break;
            case Type::Int32:
                field.value = *reinterpret_cast< const int32_t* >( input[ i ].data() );
                break;
            case Type::Uint32:
                field.value = *reinterpret_cast< const uint32_t* >( input[ i ].data() );
                break;
            case Type::Int64:
                field.value = *reinterpret_cast< const int64_t* >( input[ i ].data() );
                break;
            case Type::Uint64:
                field.value = *reinterpret_cast< const uint64_t* >( input[ i ].data() );
                break;
            case Type::Float:
                field.value = *reinterpret_cast< const float* >( input[ i ].data() );
                break;
            case Type::Double:
                field.value = *reinterpret_cast< const double* >( input[ i ].data() );
                break;
            default:
                throw std::invalid_argument( "Unsupported type " + std::to_string( static_cast< int >( reg.type ) ) + "!" );
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