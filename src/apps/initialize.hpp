#pragma once

#include "Args.hpp"
#include "rdbus/config/Config.hpp"

Args parseArguments( int, char** );
void initializeLogger( spdlog::level::level_enum );
rdbus::config::Config initializeConfig( const Args& args );
