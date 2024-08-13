#include "Interpreter.hpp"
#include "rdbus/Data.hpp"
#include <cstdint>

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
        const uint8_t msb = reg & 0xff;
        const uint8_t lsb = ( reg >> 8 ) & 0xff;
        // Swap byte order, because Modbus library does it too, so we actually result
        // with the same order as data before parsing through the library
        output.push_back( { lsb, msb } );
    }

    return output;
}

RawUint32List toRaw32BitRegisters( const RawUint16List& input )
{
    RawUint32List output;

    for ( auto it = input.begin(); it != input.end(); it += sizeof( uint16_t ) )
    {
        const auto left = *it;
        const auto right = *std::next( it );
        output.push_back( { left[ 0 ], left[ 1 ], right[ 0 ], right[ 1 ] } );
    }

    return output;
}

RawMergedList toRawMergedRegisters( const RawUint32List& input, const Registers& registers )
{
    RawMergedList output;

    int i = 0;
    for ( const auto& reg : registers )
    {
        output.push_back( { input[ i ][ 0 ], input[ i ][ 1 ], input[ i ][ 2 ], input[ i ][ 3 ] } );
        i++;

        if ( reg.byteOrder.size() > sizeof( uint32_t ) )
        {
            output.back().push_back( input[ i ][ 0 ] );
            output.back().push_back( input[ i ][ 1 ] );
            output.back().push_back( input[ i ][ 2 ] );
            output.back().push_back( input[ i ][ 3 ] );
            i++;
        }
    }

    assert( output.size() == registers.size() );

    return output;
}

BigEndianRegisters toUserInterpretation( const RawMergedList& input, const Registers& registers )
{
    BigEndianRegisters output;

    int i = 0;
    for ( const auto& reg : registers )
    {
        std::vector< uint8_t > buffer;

        for ( const auto position : reg.byteOrder )
        {
            buffer.push_back( input[ i ][ position ] );
        }

        output.emplace_back( std::move( buffer ) );

        i++;
    }

    return output;
}

SmallEndianRegisters toMachineInterpretation( const SmallEndianRegisters& input )
{
    SmallEndianRegisters output;

    for ( const auto& i : input )
    {
        auto data = i;
        std::reverse( data.begin(), data.end() );
        output.emplace_back( std::move( data ) );
    }

    return output;
}

Fields toParsedFields( const SmallEndianRegisters& input, const Registers& registers, const Timestamp& timestamp )
{
    assert( input.size() == registers.size() );

    using namespace rdbus;

    Fields output;

    int i = 0;
    for ( const auto& reg : registers )
    {
        rdbus::Output::Field field;
        field.name = reg.name;
        field.type = reg.type;
        field.timestamp = timestamp;

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
        }

        output.emplace_back( std::move( field ) );
        i++;
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