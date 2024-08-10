#include "communication/modbus/RequestPlanner.hpp"
#include "config/Register.hpp"
#include "config/Slave.hpp"
#include <gtest/gtest.h>

using namespace config;


TEST( TestRequestPlanner, TestSubsequent )
{
    Slave slave;
    slave.address = 1;
    slave.name = "slave";
    slave.pollTimeMs = std::chrono::milliseconds( 100 );

    {
        Register reg;
        reg.address = 100;
        reg.byteOrder = { 0, 1, 2, 3 };
        slave.registers.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 104;
        reg.byteOrder = { 0, 1, 2, 3, 4, 5, 6, 7 };
        slave.registers.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 112;
        reg.byteOrder = { 0, 1, 2, 3 };
        slave.registers.emplace_back( std::move( reg ) );
    }

    auto result = communication::modbus::requestPlan( slave );

    ASSERT_EQ( result.size(), 1 );
    EXPECT_EQ( result.begin()->registerAddress(), 100 );
    EXPECT_EQ( result.begin()->numberOfRegisters(), 3 );
}

TEST( TestRequestPlanner, TestWithBreaks )
{
    Slave slave;
    slave.address = 2;
    slave.name = "slave";
    slave.pollTimeMs = std::chrono::milliseconds( 100 );

    {
        Register reg;
        reg.address = 100;
        reg.byteOrder = { 0, 1, 2, 3 };
        slave.registers.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 108;
        reg.byteOrder = { 0, 1, 2, 3, 4, 5, 6, 7 };
        slave.registers.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 124;
        reg.byteOrder = { 0, 1, 2, 3 };
        slave.registers.emplace_back( std::move( reg ) );
    }

    auto result = communication::modbus::requestPlan( slave );

    ASSERT_EQ( result.size(), 3 );

    auto it = result.begin();
    EXPECT_EQ( it->registerAddress(), 100 );
    EXPECT_EQ( it->numberOfRegisters(), 1 );

    it++;
    EXPECT_EQ( it->registerAddress(), 108 );
    EXPECT_EQ( it->numberOfRegisters(), 1 );

    it++;
    EXPECT_EQ( it->registerAddress(), 124 );
    EXPECT_EQ( it->numberOfRegisters(), 1 );
}

TEST( TestRequestPlanner, TestSubsequentAndBreaks )
{
    Slave slave;
    slave.address = 2;
    slave.name = "slave";
    slave.pollTimeMs = std::chrono::milliseconds( 100 );

    {
        Register reg;
        reg.address = 100;
        reg.byteOrder = { 0, 1, 2, 3 };
        slave.registers.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 108;
        reg.byteOrder = { 0, 1, 2, 3, 4, 5, 6, 7 };
        slave.registers.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 116;
        reg.byteOrder = { 0, 1, 2, 3 };
        slave.registers.emplace_back( std::move( reg ) );
    }

    auto result = communication::modbus::requestPlan( slave );

    ASSERT_EQ( result.size(), 2 );

    auto it = result.begin();
    EXPECT_EQ( it->registerAddress(), 100 );
    EXPECT_EQ( it->numberOfRegisters(), 1 );

    it++;
    EXPECT_EQ( it->registerAddress(), 108 );
    EXPECT_EQ( it->numberOfRegisters(), 2 );
}