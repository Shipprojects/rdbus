#include "communication/Connection.hpp"
#include "OSMock.hpp"
#include "config/Serial.hpp"
#include <gtest/gtest.h>

using namespace communication;
using namespace config;

const Serial& getSettings()
{
    static Serial settings = {
        .path = "/dev/ttyS0",
        .baudRate = 9600,
        .stopBitsCount = 2,
        .parity = false
    };

    return settings;
}

TEST( TestConnection, TestConstructorSuccess )
{
    const auto& settings = getSettings();
    Connection< OSMock > connection( settings );
}
