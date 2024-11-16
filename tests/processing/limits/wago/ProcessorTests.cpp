#include "rdbus/config/processors/Limits.hpp"
#include "rdbus/config/wago/Module.hpp"
#include "rdbus/processing/limits/Data.hpp"
#include "rdbus/processing/limits/wago/Processor.hpp"
#include <gtest/gtest.h>

using namespace rdbus;
using namespace rdbus::processing;

TEST( TestLimitProcessor, Various )
{
    const std::list< config::wago::Module > modules = {
        config::wago::Module{ .name = "Module_1", .instances{ "instance_1" } },
        config::wago::Module{ .name = "Module_2", .instances{ "instance_2", "instance_3" } },
    };

    const config::processors::Limits limits{ .duration = config::processors::Limits::Minutes( 1 ),
                                             .devices = { "Module_2" } };

    std::unique_ptr< processing::Processor > processor = std::make_unique< limits::wago::Processor >( limits );

    // No readings
    {
        const auto& result = processor->process( {} );
        ASSERT_TRUE( result.empty() );
    }

    // Initial reading
    auto tp = std::chrono::system_clock::now();
    {
        const rdbus::Data reading1{
            .deviceName = "Module_1",
            .fields = {
            Data::Field{ .name = "instance_1", .value = static_cast< int16_t >( 7531 ), .timestamp = tp },
            },
        };

        const rdbus::Data reading2{
            .deviceName = "Module_2",
            .fields = {
            Data::Field{ .name = "instance_2", .value = static_cast< int16_t >( 10000 ), .timestamp = tp },
            Data::Field{ .name = "instance_3", .value = static_cast< int16_t >( -20391 ), .timestamp = tp },
            },
        };

        const auto& results = processor->process( { reading1, reading2 } );
        ASSERT_EQ( results.size(), 1 );
        const auto data = std::dynamic_pointer_cast< limits::Data >( results.front() );
        EXPECT_EQ( data->deviceName, "Module_2" );
        EXPECT_EQ( data->instanceLimits.size(), 2 );

        auto it = data->instanceLimits.begin();
        {
            EXPECT_EQ( it->name, "instance_2" );
            ASSERT_TRUE( it->max.has_value() );
            ASSERT_TRUE( it->min.has_value() );
            EXPECT_EQ( std::get< int16_t >( *it->max ), 10000 );
            EXPECT_EQ( std::get< int16_t >( *it->min ), 10000 );
        }
        {
            it++;
            EXPECT_EQ( it->name, "instance_3" );
            ASSERT_TRUE( it->max.has_value() );
            ASSERT_TRUE( it->min.has_value() );
            EXPECT_EQ( std::get< int16_t >( *it->max ), -20391 );
            EXPECT_EQ( std::get< int16_t >( *it->min ), -20391 );
        }
    }

    // Second reading, after some time
    tp += std::chrono::seconds( 10 );
    {
        const rdbus::Data reading{
            .deviceName = "Module_2",
            .fields = {
            Data::Field{ .name = "instance_2", .value = static_cast< int16_t >( 12000 ), .timestamp = tp },
            },
        };

        const auto& results = processor->process( { reading } );
        ASSERT_EQ( results.size(), 1 );
        const auto data = std::dynamic_pointer_cast< limits::Data >( results.front() );
        EXPECT_EQ( data->deviceName, "Module_2" );
        EXPECT_EQ( data->instanceLimits.size(), 2 );

        auto it = data->instanceLimits.begin();
        {
            EXPECT_EQ( it->name, "instance_2" );
            ASSERT_TRUE( it->max.has_value() );
            ASSERT_TRUE( it->min.has_value() );
            EXPECT_EQ( std::get< int16_t >( *it->max ), 12000 );
            EXPECT_EQ( std::get< int16_t >( *it->min ), 10000 );
        }
        {
            it++;
            EXPECT_EQ( it->name, "instance_3" );
            ASSERT_TRUE( it->max.has_value() );
            ASSERT_TRUE( it->min.has_value() );
            EXPECT_EQ( std::get< int16_t >( *it->max ), -20391 );
            EXPECT_EQ( std::get< int16_t >( *it->min ), -20391 );
        }
    }

    // Third reading, after some time
    tp += std::chrono::seconds( 10 );
    {
        const rdbus::Data reading{
            .deviceName = "Module_2",
            .fields = {
            Data::Field{ .name = "instance_2", .value = static_cast< int16_t >( 11500 ), .timestamp = tp },
            Data::Field{ .name = "instance_3", .value = static_cast< int16_t >( 57 ), .timestamp = tp },
            },
        };

        const auto& results = processor->process( { reading } );
        ASSERT_EQ( results.size(), 1 );
        const auto data = std::dynamic_pointer_cast< limits::Data >( results.front() );
        EXPECT_EQ( data->deviceName, "Module_2" );
        EXPECT_EQ( data->instanceLimits.size(), 2 );

        auto it = data->instanceLimits.begin();
        {
            EXPECT_EQ( it->name, "instance_2" );
            ASSERT_TRUE( it->max.has_value() );
            ASSERT_TRUE( it->min.has_value() );
            EXPECT_EQ( std::get< int16_t >( *it->max ), 12000 );
            EXPECT_EQ( std::get< int16_t >( *it->min ), 10000 );
        }
        {
            it++;
            EXPECT_EQ( it->name, "instance_3" );
            ASSERT_TRUE( it->max.has_value() );
            ASSERT_TRUE( it->min.has_value() );
            EXPECT_EQ( std::get< int16_t >( *it->max ), 57 );
            EXPECT_EQ( std::get< int16_t >( *it->min ), -20391 );
        }
    }

    // Fourth reading, instance_2 and instance_3 change their min, because it was too old
    tp += std::chrono::seconds( 50 );
    {
        const rdbus::Data reading{
            .deviceName = "Module_2",
            .fields = {
            Data::Field{ .name = "instance_2", .value = static_cast< int16_t >( 11900 ), .timestamp = tp },
            },
        };

        const auto& results = processor->process( { reading } );
        ASSERT_EQ( results.size(), 1 );
        const auto data = std::dynamic_pointer_cast< limits::Data >( results.front() );
        EXPECT_EQ( data->deviceName, "Module_2" );
        EXPECT_EQ( data->instanceLimits.size(), 2 );

        auto it = data->instanceLimits.begin();
        {
            EXPECT_EQ( it->name, "instance_2" );
            ASSERT_TRUE( it->max.has_value() );
            ASSERT_TRUE( it->min.has_value() );
            EXPECT_EQ( std::get< int16_t >( *it->max ), 12000 );
            EXPECT_EQ( std::get< int16_t >( *it->min ), 11500 );
        }
        {
            it++;
            EXPECT_EQ( it->name, "instance_3" );
            ASSERT_TRUE( it->max.has_value() );
            ASSERT_TRUE( it->min.has_value() );
            EXPECT_EQ( std::get< int16_t >( *it->max ), 57 );
            EXPECT_EQ( std::get< int16_t >( *it->min ), 57 );
        }
    }


    // Fifth reading, after a long time
    tp += std::chrono::seconds( 70 );
    {
        const rdbus::Data reading{
            .deviceName = "Module_2",
            .fields = {
            Data::Field{ .name = "instance_2", .value = static_cast< int16_t >( 5 ), .timestamp = tp },
            },
        };

        const auto& results = processor->process( { reading } );
        ASSERT_EQ( results.size(), 1 );
        const auto data = std::dynamic_pointer_cast< limits::Data >( results.front() );
        EXPECT_EQ( data->deviceName, "Module_2" );
        EXPECT_EQ( data->instanceLimits.size(), 2 );

        auto it = data->instanceLimits.begin();
        {
            EXPECT_EQ( it->name, "instance_2" );
            ASSERT_TRUE( it->max.has_value() );
            ASSERT_TRUE( it->min.has_value() );
            EXPECT_EQ( std::get< int16_t >( *it->max ), 5 );
            EXPECT_EQ( std::get< int16_t >( *it->min ), 5 );
        }
        {
            it++;
            EXPECT_EQ( it->name, "instance_3" );
            EXPECT_FALSE( it->min.has_value() );
            EXPECT_FALSE( it->max.has_value() );
        }
    }
}
