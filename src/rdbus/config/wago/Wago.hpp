#pragma once

#include "Limits.hpp"
#include "Module.hpp"
#include <list>
#include <optional>

namespace rdbus::config::wago
{

// Ethernet Industrial Protocol
struct Wago
{
    std::list< Module > modules;
    std::optional< Limits > limits;
};

} // namespace rdbus::config::wago
