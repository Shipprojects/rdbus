#pragma once

#include "Args.hpp"

Args parseArguments( int, char** );
void initializeLogger( spdlog::level::level_enum );
