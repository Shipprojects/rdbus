#pragma once

#include "Args.hpp"
#include "rdbus/Manager.hpp"
#include "rdbus/config/Config.hpp"

Args parseArguments( int, char** );
void initializeLogger();
rdbus::config::Config initializeConfig( const Args& args );
rdbus::Manager::Tasks initializeTasks( const rdbus::config::Config& );
rdbus::Manager::Output initializeOutput( const rdbus::config::Config& );
