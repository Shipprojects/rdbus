#pragma once

#include "Field.hpp"
#include <nlohmann/json.hpp>

namespace rdbus::config::nmea
{

struct Sentence
{
    std::string id;

    using Fields = std::list< Field >;
    std::list< Field > fields;
};

void from_json( const nlohmann::json& j, Sentence& x );

} // namespace rdbus::config::nmea
