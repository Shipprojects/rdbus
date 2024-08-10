#pragma once

#include "config/Serial.hpp"
#include <chrono>
#include <fcntl.h>
#include <poll.h>
#include <stdexcept>
#include <stdint.h>
#include <sys/poll.h>
#include <termios.h>
#include <vector>

namespace communication
{

// A wrapper class around OS functions that call the same OS functions
// for purposes of unit testing using mock class
struct OSWrapper
{
    int open( const char* __file, int __oflag );
    int tcgetattr( int __fd, struct termios* __termios_p ) noexcept( true );
    void cfmakeraw( struct termios* __termios_p ) noexcept( true );
    int tcflush( int __fd, int __queue_selector ) noexcept( true );
    int tcsetattr( int __fd, int __optional_actions, const struct termios* __termios_p ) noexcept( true );
    int close( int __fd );
    ssize_t write( int __fd, const void* __buf, size_t __n );
    int poll( struct pollfd* __fds, nfds_t __nfds, int __timeout );
    ssize_t read( int __fd, void* __buf, size_t __nbytes );
};

template < class OSInterfaceType >
class Connection
{
public:
    Connection( const config::Serial& settings )
    {
        const auto& path = settings.path;
        fileDescriptor = os.open( path.c_str(), O_RDWR | O_SYNC );

        if ( fileDescriptor == -1 )
        {
            throw Exception( "Cannot open serial port " + path );
        }

        // Store current port parameters in termios_ structure
        if ( os.tcgetattr( fileDescriptor, &termios_ ) != 0 )
        {
            throw Exception( "Error at tcgetattr - " + std::to_string( errno ) );
        }

        setupIO( settings );

        setBaudRate( settings.baudRate );

        // Connect
        os.tcflush( fileDescriptor, TCIFLUSH ); // Flush received data
        if ( os.tcsetattr( fileDescriptor, TCSAFLUSH, &termios_ ) != 0 ) // Make termios_ changes take effect only after data has been transmitted
        {
            throw Exception( "Error {" + std::to_string( fileDescriptor ) +
                             "} at tcsetattr - " + std::to_string( errno ) );
        }
    }

    ~Connection()
    {
        if ( fileDescriptor >= 0 )
        {
            os.close( fileDescriptor );
        }
        fileDescriptor = -1;
    }


    void sendData( const std::vector< uint8_t >& data )
    {
        // Ensure that nothing will intervene in our communication by discarding data that has been written but not transmitted
        os.tcflush( fileDescriptor, TCOFLUSH );
        // Write
        os.write( fileDescriptor, data.begin().base(), data.size() );
    }

    std::vector< uint8_t > getData( std::chrono::seconds timeout )
    {
        constexpr int maxSize = 1024;
        std::vector< uint8_t > data( maxSize );

        pollfd waitingFileDescriptor = { .fd = fileDescriptor, .events = POLLIN, .revents = POLLIN };

        // Wait for incoming data
        if ( os.poll( &waitingFileDescriptor, 1, timeout.count() ) <= 0 )
        {
            throw Timeout( "Device timeout!" );
        }

        const auto size = os.read( fileDescriptor, data.begin().base(), maxSize );

        if ( size < 0 )
        {
            throw Exception( "Device failure!" );
        }

        data.resize( size );
        data.shrink_to_fit();

        return data;
    }


    class Exception : public std::runtime_error
    {
    public:
        Exception( const std::string& what )
        : std::runtime_error( what )
        {
        }
    };

    class Timeout : public std::runtime_error
    {
    public:
        Timeout( const std::string& what )
        : std::runtime_error( what )
        {
        }
    };

private:
    termios termios_;
    int fileDescriptor;
    OSInterfaceType os;

    void setupIO( const config::Serial& settings )
    {
        // Makes the port operate in raw mode
        os.cfmakeraw( &termios_ );

        // Parity settings
        if ( settings.parity )
        {
            // Do not ignore parity errors
            termios_.c_iflag &= ~IGNPAR;
            // Enable input parity checking
            termios_.c_iflag |= INPCK;
        }
        else
        {
            termios_.c_iflag |= IGNPAR;
            termios_.c_iflag &= ~INPCK;
        }

        // Do not prefix parity error characters
        termios_.c_iflag &= ~PARMRK;

        if ( settings.stopBitsCount > 1 )
        {
            // Set two stop bits, rather than one.
            termios_.c_cflag |= CSTOPB;
        }
    }

    void setBaudRate( speed_t speed )
    {
#define setBaud( s )  \
    case s:           \
        speed = B##s; \
        break;
        switch ( speed )
        {
            setBaud( 0 );
            setBaud( 50 );
            setBaud( 75 );
            setBaud( 110 );
            setBaud( 134 );
            setBaud( 150 );
            setBaud( 200 );
            setBaud( 300 );
            setBaud( 600 );
            setBaud( 1200 );
            setBaud( 1800 );
            setBaud( 2400 );
            setBaud( 4800 );
            setBaud( 9600 );
            setBaud( 19200 );
            setBaud( 38400 );
            setBaud( 57600 );
            setBaud( 115200 );
            setBaud( 230400 );
            default:
                throw Exception( "Invalid baud rate!" );
        }
#undef setBaud

        cfsetospeed( &termios_, speed );
        cfsetispeed( &termios_, speed );
    }
};


} // namespace communication
