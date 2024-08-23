#include "OSWrapper.hpp"
#include <cerrno>
#include <fcntl.h>
#include <stdexcept>
#include <cstring>
#include <termios.h>
#include <unistd.h>

namespace rdbus::communication
{

OS::Exception::Exception( const std::string& what )
: std::runtime_error( "OS exception - " + what )
{
}

OS::Timeout::Timeout( const std::string& what )
: Exception( "Timeout - " + what )
{
}
int OSWrapper::open( const char* __file, int __oflag )
{
    return ::open( __file, __oflag );
}

int OSWrapper::tcgetattr( int __fd, struct termios* __termiosp )
{
    return ::tcgetattr( __fd, __termiosp );
}

void OSWrapper::cfmakeraw( struct termios* __termiosp )
{
    ::cfmakeraw( __termiosp );
}

int OSWrapper::tcflush( int __fd, int __queue_selector )
{
    return ::tcflush( __fd, __queue_selector );
}

int OSWrapper::tcsetattr( int __fd, int __optional_actions, const struct termios* __termiosp )
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


static std::string getError()
{
    return strerror( errno ) + std::string( " " ) + std::to_string( errno );
}

OSExtended::OSExtended( std::unique_ptr< OS > os )
: os( std::move( os ) )
{
}

int OSExtended::open( const char* __file, int __oflag )
{
    const int status = os->open( __file, __oflag );
    if ( status == -1 )
    {
        throw OS::Exception( "Error at open " + getError() );
    }

    return status;
}

int OSExtended::tcgetattr( int __fd, struct termios* __termiosp )
{
    const int status = os->tcgetattr( __fd, __termiosp );
    if ( status == -1 )
    {
        throw OS::Exception( "Error at tcgetattr " + getError() );
    }

    return status;
}

void OSExtended::cfmakeraw( struct termios* __termiosp )
{
    os->cfmakeraw( __termiosp );
}

int OSExtended::tcflush( int __fd, int __queue_selector )
{
    const int status = os->tcflush( __fd, __queue_selector );
    if ( status == -1 )
    {
        throw OS::Exception( "Error at tcflush " + getError() );
    }

    return status;
}

int OSExtended::tcsetattr( int __fd, int __optional_actions, const struct termios* __termiosp )
{
    const int status = os->tcsetattr( __fd, __optional_actions, __termiosp );
    if ( status == -1 )
    {
        throw OS::Exception( "Error at tcsetattr " + getError() );
    }

    return status;
}

int OSExtended::close( int __fd )
{
    // Must not throw!
    return os->close( __fd );
}

ssize_t OSExtended::write( int __fd, const void* __buf, size_t __n )
{
    const ssize_t status = os->write( __fd, __buf, __n );
    if ( status == -1 )
    {
        throw OS::Exception( "Error at write " + getError() );
    }

    return status;
}

int OSExtended::poll( struct pollfd* __fds, nfds_t __nfds, int __timeout )
{
    const int status = os->poll( __fds, __nfds, __timeout );
    if ( status == -1 )
    {
        throw OS::Exception( "Error at poll " + getError() );
    }

    return status;
}

ssize_t OSExtended::read( int __fd, void* __buf, size_t __nbytes )
{
    const ssize_t status = os->read( __fd, __buf, __nbytes );
    if ( status == -1 )
    {
        throw OS::Exception( "Error at read " + getError() );
    }

    return status;
}

int OSExtended::cfsetospeed( struct termios* __termios_p, speed_t __speed )
{
    const int status = os->cfsetospeed( __termios_p, __speed );
    if ( status == -1 )
    {
        throw OS::Exception( "Error at cfsetospeed " + getError() );
    }

    return status;
}

int OSExtended::cfsetispeed( struct termios* __termios_p, speed_t __speed )
{
    const int status = os->cfsetispeed( __termios_p, __speed );
    if ( status == -1 )
    {
        throw OS::Exception( "Error at cfsetispeed " + getError() );
    }

    return status;
}

} // namespace rdbus::communication
