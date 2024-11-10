#pragma once

#include "Limits.hpp"
#include "Module.hpp"
#include <list>
#include <optional>

namespace rdbus::config::eip
{

// Ethernet Industrial Protocol
struct EIP
{
    std::list< Module > modules;
    std::optional< Limits > limits;
};

} // namespace rdbus::config::eip
