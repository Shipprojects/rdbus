#pragma once

#include "rdbus/communication/OSWrapper.hpp"
#include "rdbus/config/Serial.hpp"
#include <chrono>
#include <fcntl.h>
#include <poll.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <stdint.h>
#include <sys/poll.h>
#include <termios.h>
#include <vector>

namespace rdbus::communication
{

// Interface with serial port
class Connection
{
public:
    Connection( const config::Serial& settings, std::unique_ptr< OS > os );
    ~Connection();

    void sendData( const std::vector< uint8_t >& data );
    std::vector< uint8_t > getData( std::chrono::seconds timeout );

    struct Exception : public std::runtime_error
    {
        Exception( const std::string& what );
    };

    struct Timeout : public Exception
    {
        Timeout( const std::string& what );
    };

private:
    termios termios_;
    int fileDescriptor;
    std::unique_ptr< OS > os;

    void setupIO( const config::Serial& settings );
    void setBaudRate( speed_t speed );
};


} // namespace rdbus::communication
