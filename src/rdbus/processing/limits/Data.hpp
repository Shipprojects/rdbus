#pragma once

#include "rdbus/processing/Processor.hpp"
#include "rdbus/Data.hpp"
#include <nlohmann/json.hpp>
#include <optional>

namespace rdbus::processing::limits
{

struct Data : public processing::Processor::Data
{
    std::string deviceName;

    struct FieldLimit
    {
        std::string name;

        // There are no values if there were no readings long enough
        std::optional< rdbus::Data::Field::Variant > min;
        std::optional< rdbus::Data::Field::Variant > max;
    };

    std::list< FieldLimit > instanceLimits;

    const std::string& getDeviceName() const override;
    nlohmann::json toJson() const override;
};

void to_json( nlohmann::json& j, const Data& x );

} // namespace rdbus::processing::limits
