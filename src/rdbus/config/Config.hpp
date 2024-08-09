#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace config
{

class Config
{
public:
    Config( const std::string& protocol /*const std::vector<Slave> slaves, const Output& output*/ );

private:
    std::string protocol;
    // std::vector< Slave > slaves;
    // Output output;
};

} // namespace rdbus
