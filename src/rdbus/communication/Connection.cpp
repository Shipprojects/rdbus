#include "Connection.hpp"
#include "rdbus/communication/OSWrapper.hpp"
#include <cerrno>
#include <termios.h>
#include <unistd.h>

namespace rdbus::communication
{

namespace tools
{
std::string toHexString( const uint8_t* data, int len )
{
    std::stringstream ss;
    ss << std::hex;

    for ( int i( 0 ); i < len; ++i )
    {
        ss << std::setw( 2 ) << std::setfill( '0' ) << ( int )data[ i ];
    }

    return ss.str();
}
} // namespace tools

Connection::Connection( const config::Serial& settings, std::unique_ptr< OS > os )
: os( std::move( os ) )
{
    const auto& path = settings.path;
    fileDescriptor = this->os->open( path.c_str(), O_RDWR | O_SYNC );

    if ( fileDescriptor == -1 )
    {
        throw Exception( "Cannot open serial port " + path );
    }

    // Store current port parameters in termios_ structure
    if ( this->os->tcgetattr( fileDescriptor, &termios_ ) != 0 )
    {
        throw Exception( "Error at tcgetattr - " + std::to_string( errno ) );
    }

    setupIO( settings );

    setBaudRate( settings.baudRate );

    // Connect
    this->os->tcflush( fileDescriptor, TCIFLUSH ); // Flush received data
    if ( this->os->tcsetattr( fileDescriptor, TCSAFLUSH, &termios_ ) != 0 ) // Make termios_ changes take effect only after data has been transmitted
    {
        throw Exception( "Error {" + std::to_string( fileDescriptor ) +
                         "} at tcsetattr - " + std::to_string( errno ) );
    }
}

Connection::~Connection()
{
    if ( fileDescriptor >= 0 )
    {
        os->close( fileDescriptor );
    }
    fileDescriptor = -1;
}

void Connection::sendData( const std::vector< uint8_t >& data )
{
    SPDLOG_INFO( "Sending data " + tools::toHexString( data.data(), data.size() ) );

    // Ensure that nothing will intervene in our communication by discarding data that has been written but not transmitted
    os->tcflush( fileDescriptor, TCOFLUSH );
    // Write
    os->write( fileDescriptor, data.begin().base(), data.size() );
}

std::vector< uint8_t > Connection::getData( std::chrono::seconds timeout )
{
    constexpr int maxSize = 1024;
    std::vector< uint8_t > data( maxSize );

    pollfd waitingFileDescriptor = { .fd = fileDescriptor, .events = POLLIN, .revents = POLLIN };

    // Wait for incoming data
    if ( os->poll( &waitingFileDescriptor, 1, std::chrono::milliseconds( timeout ).count() ) <= 0 )
    {
        throw Timeout( "Device timeout!" );
    }

    // Read while there is data available
    constexpr int readTimeoutMs = 100;
    ssize_t size = 0;
    do
    {
        const auto readBytes = os->read( fileDescriptor, data.data() + size, maxSize );

        if ( readBytes < 0 )
        {
            throw Exception( "Device failure!" );
        }

        size += readBytes;
    }
    while ( os->poll( &waitingFileDescriptor, 1, readTimeoutMs ) > 0 );

    data.resize( size );
    data.shrink_to_fit();

    SPDLOG_INFO( "Data received " + tools::toHexString( data.data(), data.size() ) );

    return data;
}

Connection::Exception::Exception( const std::string& what )
: std::runtime_error( what )
{
}


Connection::Timeout::Timeout( const std::string& what )
: std::runtime_error( what )
{
}

void Connection::setupIO( const config::Serial& settings )
{
    // Makes the port operate in raw mode
    os->cfmakeraw( &termios_ );

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

void Connection::setBaudRate( speed_t speed )
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

} // namespace rdbus::communication
