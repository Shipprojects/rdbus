#pragma once

#include "Manager.hpp"
#include "config/Config.hpp"
#include "config/Output.hpp"
#include <list>

namespace rdbus
{

std::list< rdbus::config::Config > initializeConfigs( const std::string& configDir );
rdbus::Manager::Output initializeOutput( const rdbus::config::Output&, const std::list< rdbus::config::Config >& );
std::list< rdbus::Manager > initializeManagers( const std::list< rdbus::config::Config >&, std::shared_ptr< out::Output > );

} // namespace rdbus
