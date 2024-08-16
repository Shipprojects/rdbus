#include "rdbus/communication/modbus/RequestPlanner.hpp"
#include "rdbus/config/Register.hpp"
#include "rdbus/config/Slave.hpp"
#include <gtest/gtest.h>

using namespace rdbus::config;
using namespace rdbus;

TEST( TestRequestPlanner, TestSubsequent )
{
    Slave slave;
    slave.id = 1;
    slave.name = "slave";
    slave.pollTimeMs = std::chrono::milliseconds( 100 );

    {
        Register reg;
        reg.address = 100;
        reg.byteOrder = { 0, 1, 2, 3 };
        reg.name = "Register_A";
        slave.inputRegisters.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 102;
        reg.byteOrder = { 0, 1, 2, 3, 4, 5, 6, 7 };
        reg.name = "Register_B";
        slave.inputRegisters.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 106;
        reg.byteOrder = { 0, 1 };
        reg.name = "Register_C";
        slave.outputRegisters.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 107;
        reg.byteOrder = { 0, 1, 2, 3 };
        reg.name = "Register_D";
        slave.outputRegisters.emplace_back( std::move( reg ) );
    }

    auto requestDescriptions = communication::modbus::requestPlan( slave );
    ASSERT_EQ( requestDescriptions.size(), 2 );

    const auto& description = *requestDescriptions.begin();
    EXPECT_EQ( description.request.registerAddress(), 106 );
    EXPECT_EQ( description.request.numberOfRegisters(), 3 );

    auto reg = description.registers.begin();
    ASSERT_EQ( description.registers.size(), 2 );

    EXPECT_EQ( reg->name, "Register_C" );
    reg++;
    EXPECT_EQ( reg->name, "Register_D" );
    reg++;
    EXPECT_EQ( reg->name, "Register_A" );
    reg++;
    EXPECT_EQ( reg->name, "Register_B" );
}

TEST( TestRequestPlanner, TestWithBreaks )
{
    Slave slave;
    slave.id = 2;
    slave.name = "slave";
    slave.pollTimeMs = std::chrono::milliseconds( 100 );

    {
        Register reg;
        reg.address = 100;
        reg.byteOrder = { 0, 1, 2, 3 };
        reg.name = "Register_A";
        slave.inputRegisters.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 108;
        reg.byteOrder = { 0, 1, 2, 3, 4, 5, 6, 7 };
        reg.name = "Register_B";
        slave.outputRegisters.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 114;
        reg.byteOrder = { 0, 1, 2, 3 };
        reg.name = "Register_C";
        slave.inputRegisters.emplace_back( std::move( reg ) );
    }

    auto requestDescriptions = communication::modbus::requestPlan( slave );
    ASSERT_EQ( requestDescriptions.size(), 3 );

    auto it = requestDescriptions.begin();
    EXPECT_EQ( it->request.registerAddress(), 108 );
    EXPECT_EQ( it->request.numberOfRegisters(), 4 );
    ASSERT_EQ( it->registers.size(), 1 );
    EXPECT_EQ( it->registers.begin()->name, "Register_B" );

    it++;
    EXPECT_EQ( it->request.registerAddress(), 100 );
    EXPECT_EQ( it->request.numberOfRegisters(), 2 );
    ASSERT_EQ( it->registers.size(), 1 );
    EXPECT_EQ( it->registers.begin()->name, "Register_A" );


    it++;
    EXPECT_EQ( it->request.registerAddress(), 114 );
    EXPECT_EQ( it->request.numberOfRegisters(), 2 );
    ASSERT_EQ( it->registers.size(), 1 );
    EXPECT_EQ( it->registers.begin()->name, "Register_C" );
}

TEST( TestRequestPlanner, TestSubsequentAndBreaks )
{
    Slave slave;
    slave.id = 2;
    slave.name = "slave";
    slave.pollTimeMs = std::chrono::milliseconds( 100 );

    {
        Register reg;
        reg.address = 100;
        reg.byteOrder = { 0, 1, 2, 3 };
        reg.name = "Register_A";
        slave.inputRegisters.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 102;
        reg.byteOrder = { 0, 1 };
        reg.name = "Register_B";
        slave.inputRegisters.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 104;
        reg.byteOrder = { 0, 1, 2, 3, 4, 5, 6, 7 };
        reg.name = "Register_C";
        slave.inputRegisters.emplace_back( std::move( reg ) );
    }

    {
        Register reg;
        reg.address = 108;
        reg.byteOrder = { 0, 1, 2, 3 };
        reg.name = "Register_D";
        slave.outputRegisters.emplace_back( std::move( reg ) );
    }

    auto requestDescriptions = communication::modbus::requestPlan( slave );

    ASSERT_EQ( requestDescriptions.size(), 2 );

    auto it = requestDescriptions.begin();
    EXPECT_EQ( it->request.registerAddress(), 108 );
    EXPECT_EQ( it->request.numberOfRegisters(), 2 );
    ASSERT_EQ( it->registers.size(), 1 );
    EXPECT_EQ( it->registers.begin()->name, "Register_D" );

    it++;
    EXPECT_EQ( it->request.registerAddress(), 100 );
    EXPECT_EQ( it->request.numberOfRegisters(), 3 );
    ASSERT_EQ( it->registers.size(), 2 );
    EXPECT_EQ( it->registers.begin()->name, "Register_A" );
    EXPECT_EQ( std::next( it->registers.begin() )->name, "Register_B" );

    it++;
    EXPECT_EQ( it->request.registerAddress(), 104 );
    EXPECT_EQ( it->request.numberOfRegisters(), 4 );
    ASSERT_EQ( it->registers.size(), 1 );
    EXPECT_EQ( it->registers.begin()->name, "Register_C" );
}