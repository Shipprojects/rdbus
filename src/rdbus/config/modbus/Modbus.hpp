#pragma once

#include "Slave.hpp"
#include <list>

namespace rdbus::config::modbus
{

struct Modbus
{
    using Slaves = std::list< Slave >;
    Slaves slaves;
};

} // namespace rdbus::config::modbus
