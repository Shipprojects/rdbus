#include "rdbus/communication/modbus/Communicator.hpp"
#include "rdbus/config/Register.hpp"
#include "rdbus/config/Slave.hpp"
#include <gtest/gtest.h>

using namespace rdbus::config;
using namespace rdbus;

// TEST( TestRequestPlanner, TestSubsequent )
// {
//     Slave slave;
//     slave.id = 1;
//     slave.name = "slave";
//     slave.pollTimeMs = std::chrono::milliseconds( 100 );

//     {
//         Register reg;
//         reg.address = 100;
//         reg.byteOrder = { 0, 1, 2, 3 };
//         reg.name = "Register_A";
//         slave.registers.emplace_back( std::move( reg ) );
//     }

//     {
//         Register reg;
//         reg.address = 102;
//         reg.byteOrder = { 0, 1, 2, 3, 4, 5, 6, 7 };
//         reg.name = "Register_B";
//         slave.registers.emplace_back( std::move( reg ) );
//     }

//     {
//         Register reg;
//         reg.address = 106;
//         reg.byteOrder = { 0, 1 };
//         reg.name = "Register_C";
//         slave.registers.emplace_back( std::move( reg ) );
//     }

//     {
//         Register reg;
//         reg.address = 107;
//         reg.byteOrder = { 0, 1, 2, 3 };
//         reg.name = "Register_D";
//         slave.registers.emplace_back( std::move( reg ) );
//     }

//     auto requestDescriptions = communication::modbus::requestPlan( slave );
//     ASSERT_EQ( requestDescriptions.size(), 1 );

//     const auto& description = *requestDescriptions.begin();
//     EXPECT_EQ( description.request.registerAddress(), 100 );
//     EXPECT_EQ( description.request.numberOfRegisters(), 9 );

//     auto reg = description.registers.begin();
//     ASSERT_EQ( description.registers.size(), 4 );

//     EXPECT_EQ( reg->name, "Register_A" );
//     reg++;
//     EXPECT_EQ( reg->name, "Register_B" );
//     reg++;
//     EXPECT_EQ( reg->name, "Register_C" );
//     reg++;
//     EXPECT_EQ( reg->name, "Register_D" );
// }
