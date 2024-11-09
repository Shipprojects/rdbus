#pragma once

#include "Limits.hpp"
#include "Module.hpp"
#include <list>
#include <optional>

namespace rdbus::config::ip
{

// Industrial Protocol
struct IP
{
    std::list< Module > modules;
    std::optional< Limits > limits;
};

} // namespace rdbus::config::ip
