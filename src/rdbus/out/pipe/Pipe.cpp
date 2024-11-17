#include "Pipe.hpp"
#include "rdbus/Data.hpp"
#include <iostream>
#include <nlohmann/json.hpp>

using namespace nlohmann;

namespace rdbus::out::pipe
{

void Pipe::send( const std::list< rdbus::Data >& list )
{
    // This is thread safe operation - the only common resource
    // that gets accessed is std::cout and it is also thread safe
    const json j = list;
    constexpr auto indent = 4;
    std::cout << j.dump( indent ) + '\n';
}

void Pipe::send( const processing::Processor::OutputList& list, processing::Name name )
{
    auto j = json::array();
    for ( const auto& entry : list )
    {
        j.emplace_back( entry->toJson() );
    }

    if ( !j.empty() )
    {
        constexpr auto indent = 4;
        std::cout << j.dump( indent ) + '\n';
    }
}
} // namespace rdbus::out::pipe
