#include "Pipe.hpp"
#include "rdbus/Data.hpp"
#include <iostream>
#include <nlohmann/json.hpp>

namespace rdbus::out::pipe
{

void Pipe::send( const std::list< rdbus::Data >& list )
{
    // This is thread safe operation - the only common resource
    // that gets accessed is std::cout and it is also thread safe
    const nlohmann::json j = list;
    constexpr auto indent = 4;
    std::cout << j.dump( indent ) + '\n';
}

void Pipe::send( const processing::Base::OutputList& list, processing::Name name )
{
    for ( const auto& entry : list )
    {
        const nlohmann::json j = entry->toJson();
        constexpr auto indent = 4;
        std::cout << j.dump( indent ) + '\n';
    }
}
} // namespace rdbus::out::pipe
