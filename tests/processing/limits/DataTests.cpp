#include "rdbus/processing/limits/Data.hpp"
#include <gtest/gtest.h>

using namespace rdbus::processing::limits;
using namespace nlohmann::literals;

TEST( TestProcessingData, Test )
{
    Data data;
    data.deviceName = "Sensor";
    data.instanceLimits = {
        {
        "instance_1",
        100,
        100,
        },
        {
        "instance_2",
        -2547,
        259,
        },
        Data::FieldLimit{
        "instance_3",
        },
    };

    const nlohmann::json j = data;

    EXPECT_EQ( j, R"(
        {
            "device": "Sensor",
            "fields": [
                {
                    "name": "instance_1",
                    "min": 100,
                    "max": 100
                },
                {
                    "name": "instance_2",
                    "min": -2547,
                    "max": 259
                },
                {
                    "name": "instance_3",
                    "min": null,
                    "max": null
                }
            ]
        })"_json );
}
