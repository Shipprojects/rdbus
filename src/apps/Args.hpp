#pragma once

#include <string>

struct Args
{
    std::string configPath;
    std::string devPath;
    bool forceStdout = false;
};
