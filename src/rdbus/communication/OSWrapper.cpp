#include "OSWrapper.hpp"
#include <cerrno>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

namespace rdbus::communication
{

int OSWrapper::open( const char* __file, int __oflag )
{
    return ::open( __file, __oflag );
}

int OSWrapper::tcgetattr( int __fd, struct termios* __termiosp ) noexcept( true )
{
    return ::tcgetattr( __fd, __termiosp );
}

void OSWrapper::cfmakeraw( struct termios* __termiosp ) noexcept( true )
{
    ::cfmakeraw( __termiosp );
}

int OSWrapper::tcflush( int __fd, int __queue_selector ) noexcept( true )
{
    return ::tcflush( __fd, __queue_selector );
}

int OSWrapper::tcsetattr( int __fd, int __optional_actions, const struct termios* __termiosp ) noexcept( true )
{
    return ::tcsetattr( __fd, __optional_actions, __termiosp );
}

int OSWrapper::close( int __fd )
{
    return ::close( __fd );
}

ssize_t OSWrapper::write( int __fd, const void* __buf, size_t __n )
{
    return ::write( __fd, __buf, __n );
}

int OSWrapper::poll( struct pollfd* __fds, nfds_t __nfds, int __timeout )
{
    return ::poll( __fds, __nfds, __timeout );
}

ssize_t OSWrapper::read( int __fd, void* __buf, size_t __nbytes )
{
    return ::read( __fd, __buf, __nbytes );
}

int OSWrapper::cfsetospeed( struct termios* __termios_p, speed_t __speed )
{
    return ::cfsetospeed( __termios_p, __speed );
}

int OSWrapper::cfsetispeed( struct termios* __termios_p, speed_t __speed )
{
    return ::cfsetispeed( __termios_p, __speed );
}

} // namespace rdbus::communication
