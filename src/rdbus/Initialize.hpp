#pragma once

#include "Manager.hpp"
#include "config/Config.hpp"
#include "config/Output.hpp"
#include <list>

namespace rdbus
{

std::list< rdbus::config::Config > initializeConfig( const std::string& configDir );
rdbus::Manager::Output initializeOutput( const rdbus::config::Output& );
rdbus::Manager::Tasks initializeTasks( const rdbus::config::Config& );

} // namespace rdbus
