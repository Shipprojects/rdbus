#pragma once

#include "rdbus/config/Output.hpp"
#include "spdlog/common.h"
#include <spdlog/spdlog.h>
#include <string>

struct Args
{
    std::string configDir;
    rdbus::config::Output output;
    spdlog::level::level_enum logLevel = spdlog::level::level_enum::off;
};
