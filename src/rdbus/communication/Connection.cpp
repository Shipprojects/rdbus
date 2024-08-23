#include "Connection.hpp"
#include "rdbus/communication/OSWrapper.hpp"
#include "rdbus/config/Serial.hpp"
#include <cerrno>
#include <termios.h>
#include <unistd.h>

namespace rdbus::communication
{

static std::string toHexString( const uint8_t* data, int len )
{
    std::stringstream ss;
    ss << std::hex;

    for ( int i = 0; i < len; ++i )
    {
        ss << std::setw( 2 ) << std::setfill( '0' ) << static_cast< int >( data[ i ] );
    }

    return ss.str();
}

Connection::Connection( const config::Serial& settings, std::unique_ptr< OS > os )
: os( std::move( os ) ),
  responseTimeout( settings.responseTimeout ),
  lineTimeout( settings.lineTimeout )
{
    const auto& path = settings.path;
    fileDescriptor = this->os.open( path.c_str(), O_RDWR | O_SYNC );

    // Store current port parameters in termios_ structure
    this->os.tcgetattr( fileDescriptor, &termios_ );

    setupIO( settings );

    setBaudRate( settings.baudRate );

    // Flush dangling data
    this->os.tcflush( fileDescriptor, TCIFLUSH );

    // Store termios_ setup
    this->os.tcsetattr( fileDescriptor, TCSADRAIN, &termios_ );
}

Connection::~Connection()
{
    if ( fileDescriptor >= 0 )
    {
        os.close( fileDescriptor );
    }
    fileDescriptor = -1;
}

void Connection::sendData( const std::vector< uint8_t >& data )
{
    SPDLOG_INFO( "Sending data " + toHexString( data.data(), data.size() ) );

    // Ensure that nothing will intervene in our communication by discarding data that has been written but not transmitted
    os.tcflush( fileDescriptor, TCOFLUSH );

    // Write
    os.write( fileDescriptor, data.begin().base(), data.size() );
}

std::vector< uint8_t > Connection::getData()
{
    // Arbitrary large max size
    constexpr int maxSize = 1024;
    std::vector< uint8_t > data( maxSize );

    pollfd waitingFileDescriptor = { .fd = fileDescriptor, .events = POLLIN, .revents = POLLIN };

    // Wait for incoming data
    if ( os.poll( &waitingFileDescriptor, 1, responseTimeout.count() ) == 0 )
    {
        throw OS::Timeout( "Device timeout!" );
    }

    ssize_t size = 0;
    // Read while there is data available
    do
    {
        size += os.read( fileDescriptor, data.data() + size, maxSize );
    }
    while ( os.poll( &waitingFileDescriptor, 1, lineTimeout.count() ) > 0 );

    data.resize( size );
    data.shrink_to_fit();

    SPDLOG_INFO( "Data received " + toHexString( data.data(), data.size() ) );

    return data;
}

void Connection::setupIO( const config::Serial& settings )
{
    // Sets up some default parameters
    os.cfmakeraw( &termios_ );

    // Parity settings
    using Parity = config::Serial::Parity;
    switch ( settings.parity )
    {
        case Parity::Even:
            // Disable odd parity to get even parity
            termios_.c_cflag &= ~PARODD;
            break;
        case Parity::Odd:
            // Enable odd parity
            termios_.c_cflag |= PARODD;
            break;
        case Parity::None:
        default:
            // Disable parity checking
            termios_.c_cflag &= ~PARENB;
            break;
    }

    if ( settings.parity == Parity::Even || settings.parity == Parity::Odd )
    {
        // Enable parity generation on output
        termios_.c_cflag |= PARENB;
        // Do not ignore parity errors
        termios_.c_iflag &= ~IGNPAR;
        // Enable input parity checking
        termios_.c_iflag |= INPCK;
        // Do not prefix parity error bytes
        termios_.c_iflag &= ~PARMRK;
    }

    if ( settings.stopBitsCount == 2 )
    {
        // Set two stop bits, rather than one
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
            throw OS::Exception( "Invalid baud rate!" );
    }
#undef setBaud

    os.cfsetospeed( &termios_, speed );
    os.cfsetispeed( &termios_, speed );
}

} // namespace rdbus::communication
