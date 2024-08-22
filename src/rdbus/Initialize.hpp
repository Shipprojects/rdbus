#pragma once

#include "Manager.hpp"
#include "config/Config.hpp"

namespace rdbus
{

rdbus::Manager::Tasks initializeTasks( const rdbus::config::Config& );
rdbus::Manager::Output initializeOutput( const rdbus::config::Config& );

} // namespace rdbus
