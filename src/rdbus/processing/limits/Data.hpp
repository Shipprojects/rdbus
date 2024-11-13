#pragma once

#include "rdbus/processing/Base.hpp"
#include <nlohmann/json.hpp>
#include <optional>

namespace rdbus::processing::limits
{

struct Data : public processing::Base::Data
{
    std::string moduleName;

    struct InstanceLimit
    {
        std::string instanceName;

        // There are no values if there were no readings long enough
        std::optional< int > min;
        std::optional< int > max;
    };

    std::list< InstanceLimit > instanceLimits;
};

void to_json( nlohmann::json& j, const Data& x );

} // namespace rdbus::processing::limits
