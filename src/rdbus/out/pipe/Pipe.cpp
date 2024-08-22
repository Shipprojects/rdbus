#include "Pipe.hpp"
#include "rdbus/Data.hpp"
#include <iostream>
#include <nlohmann/json.hpp>

namespace rdbus::out::pipe
{

void Pipe::send( const rdbus::Data& data )
{
    const nlohmann::json j = data;
    constexpr auto indent = 4;
    std::cout << j.dump( indent ) + '\n';
}

void Pipe::send( const std::list< rdbus::Data >& list )
{
    // This is thread safe operation - the only common resource
    // that gets accessed is std::cout and it is also thread safe
    for ( const auto& data : list )
    {
        send( data );
    }
}

} // namespace rdbus::out::pipe
