#pragma once

#include "rdbus/communication/OSWrapper.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sys/poll.h>

class OSMock : public rdbus::communication::OS
{
public:
    MOCK_METHOD( int, open, ( const char*, int ), () );
    MOCK_METHOD( int, tcgetattr, ( int, struct termios* ), ( noexcept( true ) ) );
    MOCK_METHOD( void, cfmakeraw, ( struct termios* ), ( noexcept( true ) ) );
    MOCK_METHOD( int, tcflush, ( int, int ), ( noexcept( true ) ) );
    MOCK_METHOD( int, tcsetattr, ( int, int, const struct termios* ), ( noexcept( true ) ) );
    MOCK_METHOD( int, close, ( int ), () );
    MOCK_METHOD( ssize_t, write, ( int, const void*, size_t ), () );
    MOCK_METHOD( int, poll, ( struct pollfd*, nfds_t, int ), () );
    MOCK_METHOD( ssize_t, read, ( int, void*, size_t ), () );
    MOCK_METHOD( int, cfsetospeed, ( struct termios*, speed_t ), () );
    MOCK_METHOD( int, cfsetispeed, ( struct termios*, speed_t ), () );
};
