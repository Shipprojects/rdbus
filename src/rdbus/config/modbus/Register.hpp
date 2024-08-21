#pragma once

#include "rdbus/Data.hpp"
#include <list>
#include <nlohmann/json.hpp>
#include <string>

namespace rdbus::config::modbus
{

struct Register
{
    std::string name;
    int address = 0;

    // The order of bytes in register described in terms of Big Endian. E.g. int32 Big Endian order is 0,1,2,3 where
    // 0 is the most significant byte and 3 is the least significant byte. If the order is described as 2,3,0,1, it
    // means that the first byte will go to 2nd position, second byte will go to 3rd position, third
    // byte will go to 0th position, last byte will go to 1st position.
    std::list< int > byteOrder;

    rdbus::Type type;
};

void from_json( const nlohmann::json& j, Register& x );

} // namespace rdbus::config
