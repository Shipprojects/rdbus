#include "Pipe.hpp"
#include "rdbus/Data.hpp"
#include <iostream>
#include <nlohmann/json.hpp>

namespace rdbus::out::pipe
{

Pipe::Pipe()
{
}

void Pipe::send( const rdbus::Data& data )
{
    const nlohmann::json j = data;
    constexpr auto indent = 4;
    std::cout << j.dump( indent ) << std::endl;
}

void Pipe::send( const std::list< rdbus::Data >& list )
{
    for ( const auto& data : list )
    {
        send( data );
    }
}

} // namespace rdbus::out::pipe
