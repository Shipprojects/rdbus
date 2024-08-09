#pragma once

#include <nlohmann/json.hpp>

nlohmann::json getJsonFromPath( const std::string& path );