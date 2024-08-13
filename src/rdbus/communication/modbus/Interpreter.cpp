#include "Interpreter.hpp"
#include "rdbus/Data.hpp"
#include <cstdint>
#include <exception>
#include <stdexcept>

namespace communication::modbus::interpreter
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

RawUint32List toRaw32BitRegisters( const RawUint16List& input )
{
    RawUint32List output;

    for ( auto it = input.begin(); it != input.end(); it += sizeof( uint16_t ) )
    {
        const auto msw = *it;
        const auto lsw = *std::next( it );
        output.push_back( { msw[ 0 ], msw[ 1 ], lsw[ 0 ], lsw[ 1 ] } );
    }

    return output;
}

RawMergedList toRawMergedRegisters( const RawUint32List& input, const Registers& registers )
{
    RawMergedList output;

    int i = 0;
    for ( const auto& reg : registers )
    {
        std::vector< uint8_t > word;

        word.insert( word.end(), input[ i ].begin(), input[ i ].end() );
        i++;

        if ( reg.byteOrder.size() > sizeof( uint32_t ) )
        {
            word.insert( word.end(), input[ i ].begin(), input[ i ].end() );
            i++;
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
        rdbus::Output::Field field = {
            .name = reg.name,
            .type = reg.type,
            .timestamp = timestamp
        };

        switch ( reg.type )
        {
            case Type::Int32:
                field.value = *reinterpret_cast< const int32_t* >( input[ i ].data() );
                break;
            case Type::Uint32:
                field.value = *reinterpret_cast< const uint32_t* >( input[ i ].data() );
                break;
            case Type::Float:
                field.value = *reinterpret_cast< const float* >( input[ i ].data() );
                break;
            case Type::Double:
                field.value = *reinterpret_cast< const double* >( input[ i ].data() );
                break;
            default:
                throw std::invalid_argument( "Unknown argument!" );
                break;
        }

        output.emplace_back( std::move( field ) );
    }

    return output;
}

} // namespace tools

std::list< rdbus::Output::Field > parse( const MB::ModbusResponse& response,
                                         const std::list< config::Register >& registers,
                                         const rdbus::Output::Field::Timestamp& timestamp )
{
    using namespace tools;

    const auto& rawUint16List = toRaw16BitRegisters( response.registerValues() );
    const auto& rawUint32List = toRaw32BitRegisters( rawUint16List );
    const auto& rawMergedList = toRawMergedRegisters( rawUint32List, registers );
    const auto& bigEndianRegisters = toUserInterpretation( rawMergedList, registers );
    const auto& littleEndianRegisters = toMachineInterpretation( bigEndianRegisters );
    const auto& fields = toParsedFields( littleEndianRegisters, registers, timestamp );
    return fields;
}

} // namespace communication::modbus::interpreter