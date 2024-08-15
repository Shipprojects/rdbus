#pragma once

#include "spdlog/common.h"
#include <spdlog/spdlog.h>
#include <string>

struct Args
{
    std::string configPath;
    std::string devPath;
    bool forceStdout = false;
    spdlog::level::level_enum logLevel = spdlog::level::level_enum::off;
};
