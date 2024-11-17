#pragma once

#include "Module.hpp"
#include <list>

namespace rdbus::config::wago
{

struct Wago
{
    using Modules = std::list< Module >;
    Modules modules;
};

} // namespace rdbus::config::wago
