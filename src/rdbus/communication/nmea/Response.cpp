#include "Response.hpp"
#include <algorithm>
#include <sstream>

namespace rdbus::communication::nmea
{

Response::Exception::Exception( const std::string& what )
: rdbus::Exception( "NMEA parse error - " + what )
{
}

// Extracts device id from data before the first comma
static std::string extractTalkerID( const std::string& token )
{
    // The device id comes after '$' and before last 3 characters which is sentence id
    return token.substr( 1, token.size() - 1 - 3 );
}

// Extracts sentence id from data before the first comma
static std::string extractSentenceID( const std::string& token )
{
    // The sentence id is last 3 characters
    return token.substr( token.size() - 3 );
}

static uint8_t computeChecksum( std::string data )
{
    // The data for checksum generation starts after $ and ends before *
    const int begin = 1;
    const int end = data.find( '*' );

    data = data.substr( begin, end - begin );

    // The NMEA checksum is exclusive OR of all characters
    uint8_t checksum = 0;
    for ( const auto c : data )
    {
        checksum ^= c;
    }

    return checksum;
}

static bool checksum( const std::string& data )
{
    const uint8_t checksum = computeChecksum( data );

    // The beginning of checksum is after *
    const int begin = data.find( '*' ) + 1;
    const auto receivedChecksumString = data.substr( begin, 2 );

    // Convert from hex string to int
    int receivedChecksum = 0;
    std::stringstream ss;
    ss << std::hex << receivedChecksumString;
    ss >> receivedChecksum;

    return checksum == receivedChecksum;
}

static void validate( const std::string& data, bool validateChecksum )
{
    if ( std::ranges::count( data, '*' ) > 1 || // If there are more than one *
         *data.begin() != '$' || // If the first character is not $
         // If the end of the message is not CRLF
         *( std::next( data.rbegin() ) ) != '\r' ||
         *data.rbegin() != '\n' )
    {
        throw Response::Exception( "Invalid message structure!" );
    }

    if ( validateChecksum )
    {
        // If could not find the checksum part
        if ( data.find( '*' ) == std::string::npos )
        {
            throw Response::Exception( "No checksum found!" );
        }

        if ( !checksum( data ) )
        {
            throw Response::Exception( "Checksum error!" );
        }
    }
}

// Remove redundant data (checksum and CRLF) from the last token
void cleanBack( std::string& token )
{
    const int delimiterPosition = token.find( '*' );
    if ( delimiterPosition != std::string::npos )
    {
        token = token.substr( 0, delimiterPosition );
    }
    else
    {
        token = token.substr( 0, token.size() - 2 );
    }
}

Response::Response( const std::vector< uint8_t >& data, bool validateChecksum )
{
    const auto str = std::string( data.begin(), data.end() );

    validate( str, validateChecksum );

    std::stringstream ss( str );

    // Tokenize the input string by comma delimiter
    std::string token;
    std::vector< std::string > tokens;
    const char delimiter = ',';
    while ( getline( ss, token, delimiter ) )
    {
        tokens.push_back( token );
    }

    cleanBack( tokens.back() );

    talkerID = extractTalkerID( tokens[ 0 ] );
    sentenceId = extractSentenceID( tokens[ 0 ] );
    // Do not include the talker/sentence ID part in fields
    fields = Fields( std::next( tokens.begin() ), tokens.end() );
}

const std::string& Response::getTalkerID() const
{
    return talkerID;
}

const std::string& Response::getSentenceID() const
{
    return sentenceId;
}

auto Response::getFields() const -> const Fields&
{
    return fields;
}

} // namespace rdbus::communication::nmea
