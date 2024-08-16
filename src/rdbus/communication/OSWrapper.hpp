#pragma once

#include <cstddef>
#include <sys/poll.h>
#include <sys/types.h>
#include <termios.h>

namespace rdbus::communication
{

struct OS
{
    virtual int open( const char* __file, int __oflag ) = 0;
    virtual int tcgetattr( int __fd, struct termios* __termios_p ) noexcept( true ) = 0;
    virtual void cfmakeraw( struct termios* __termios_p ) noexcept( true ) = 0;
    virtual int tcflush( int __fd, int __queue_selector ) noexcept( true ) = 0;
    virtual int tcsetattr( int __fd, int __optional_actions, const struct termios* __termios_p ) noexcept( true ) = 0;
    virtual int close( int __fd ) = 0;
    virtual ssize_t write( int __fd, const void* __buf, size_t __n ) = 0;
    virtual int poll( struct pollfd* __fds, nfds_t __nfds, int __timeout ) = 0;
    virtual ssize_t read( int __fd, void* __buf, size_t __nbytes ) = 0;
    virtual int cfsetospeed( struct termios* __termios_p, speed_t __speed ) = 0;
    virtual int cfsetispeed( struct termios* __termios_p, speed_t __speed ) = 0;
    virtual ~OS() = default;
};

// A wrapper class around OS functions that calls the same OS functions
// for purposes of unit testing using mock class
struct OSWrapper : public OS
{
    int open( const char* __file, int __oflag ) override;
    int tcgetattr( int __fd, struct termios* __termios_p ) noexcept( true ) override;
    void cfmakeraw( struct termios* __termios_p ) noexcept( true ) override;
    int tcflush( int __fd, int __queue_selector ) noexcept( true ) override;
    int tcsetattr( int __fd, int __optional_actions, const struct termios* __termios_p ) noexcept( true ) override;
    int close( int __fd ) override;
    ssize_t write( int __fd, const void* __buf, size_t __n ) override;
    int poll( struct pollfd* __fds, nfds_t __nfds, int __timeout ) override;
    ssize_t read( int __fd, void* __buf, size_t __nbytes ) override;
    int cfsetospeed( struct termios* __termios_p, speed_t __speed ) override;
    int cfsetispeed( struct termios* __termios_p, speed_t __speed ) override;
};

} // namespace rdbus::communication
