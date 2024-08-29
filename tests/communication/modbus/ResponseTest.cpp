#include "MB/modbusException.hpp"
#include <MB/modbusResponse.hpp>
#include <gtest/gtest.h>

// While it is not our responsibility to test external Modbus library, it's exception generation/handling is not
// properly tested, so we have to do it ourselves. Just in case.

using namespace MB;

TEST( MBResponseTest, IllegalFunction )
{
    const std::vector< uint8_t > data = { 0x02, 0x81, 0x02, 0x31, 0x91 };
    EXPECT_THROW( ModbusResponse::fromRawCRC( data ), ModbusException );
}

TEST( MBResponseTest, IllegalDataAddress )
{
    const std::vector< uint8_t > data = { 0x02, 0x82, 0x02, 0x31, 0x61 };
    EXPECT_THROW( ModbusResponse::fromRawCRC( data ), ModbusException );
}

TEST( MBResponseTest, IllegalDataValue )
{
    const std::vector< uint8_t > data = { 0x02, 0x83, 0x02, 0x30, 0xf1 };
    EXPECT_THROW( ModbusResponse::fromRawCRC( data ), ModbusException );
}

TEST( MBResponseTest, SlaveDeviceFailure )
{
    const std::vector< uint8_t > data = { 0x02, 0x84, 0x02, 0x32, 0xc1 };
    EXPECT_THROW( ModbusResponse::fromRawCRC( data ), ModbusException );
}

TEST( MBResponseTest, NegativeAcknowledge )
{
    const std::vector< uint8_t > data = { 0x02, 0x87, 0x02, 0x32, 0x31 };
    EXPECT_THROW( ModbusResponse::fromRawCRC( data ), ModbusException );
}

TEST( MBResponseTest, CRCMismatch )
{
    const std::vector< uint8_t > data = { 0x02, 0x87, 0x02, 0x32, 0x32 };
    EXPECT_THROW( ModbusResponse::fromRawCRC( data ), ModbusException );
}
