#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/types.h>
#include <termios.h>

namespace rdbus::communication
{

struct OS
{
    virtual int open( const char* __file, int __oflag ) = 0;
    virtual int tcgetattr( int __fd, struct termios* __termios_p ) = 0;
    virtual void cfmakeraw( struct termios* __termios_p ) = 0;
    virtual int tcflush( int __fd, int __queue_selector ) = 0;
    virtual int tcsetattr( int __fd, int __optional_actions, const struct termios* __termios_p ) = 0;
    virtual int close( int __fd ) = 0;
    virtual ssize_t write( int __fd, const void* __buf, size_t __n ) = 0;
    virtual int poll( struct pollfd* __fds, nfds_t __nfds, int __timeout ) = 0;
    virtual ssize_t read( int __fd, void* __buf, size_t __nbytes ) = 0;
    virtual int cfsetospeed( struct termios* __termios_p, speed_t __speed ) = 0;
    virtual int cfsetispeed( struct termios* __termios_p, speed_t __speed ) = 0;
    virtual ~OS() = default;

    struct Exception : public std::runtime_error
    {
        explicit Exception( const std::string& what );
    };

    struct Timeout : public Exception
    {
        explicit Timeout( const std::string& what );
    };
};

// A wrapper class around OS functions that calls the same OS functions
// for purposes of unit testing using mock class
struct OSWrapper : public OS
{
    int open( const char* __file, int __oflag ) override;
    int tcgetattr( int __fd, struct termios* __termios_p ) override;
    void cfmakeraw( struct termios* __termios_p ) override;
    int tcflush( int __fd, int __queue_selector ) override;
    int tcsetattr( int __fd, int __optional_actions, const struct termios* __termios_p ) override;
    int close( int __fd ) override;
    ssize_t write( int __fd, const void* __buf, size_t __n ) override;
    int poll( struct pollfd* __fds, nfds_t __nfds, int __timeout ) override;
    ssize_t read( int __fd, void* __buf, size_t __nbytes ) override;
    int cfsetospeed( struct termios* __termios_p, speed_t __speed ) override;
    int cfsetispeed( struct termios* __termios_p, speed_t __speed ) override;
};

// A wrapper class around OS methods (both wrapper and mock) that checks for return
// values and throws if the return value is -1
struct OSExtended : public OS
{
    explicit OSExtended( std::unique_ptr< OS > os );

    int open( const char* __file, int __oflag ) override;
    int tcgetattr( int __fd, struct termios* __termios_p ) override;
    void cfmakeraw( struct termios* __termios_p ) override;
    int tcflush( int __fd, int __queue_selector ) override;
    int tcsetattr( int __fd, int __optional_actions, const struct termios* __termios_p ) override;
    int close( int __fd ) override;
    ssize_t write( int __fd, const void* __buf, size_t __n ) override;
    int poll( struct pollfd* __fds, nfds_t __nfds, int __timeout ) override;
    ssize_t read( int __fd, void* __buf, size_t __nbytes ) override;
    int cfsetospeed( struct termios* __termios_p, speed_t __speed ) override;
    int cfsetispeed( struct termios* __termios_p, speed_t __speed ) override;

private:
    std::unique_ptr< OS > os;
};

} // namespace rdbus::communication
