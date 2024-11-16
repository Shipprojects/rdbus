#include "Interpreter.hpp"
#include "rdbus/Data.hpp"
#include <exception>

namespace rdbus::communication::nmea::interpreter
{

namespace tools
{

Exception::Exception( const std::string& what )
: rdbus::communication::interpreter::Exception( "NMEA - " + what )
{
}

const Sentence& findSentence( const tools::Sentences& sentences, const std::string& sentenceId )
{
    const auto it = std::find_if( sentences.begin(), sentences.end(),
                                  [ & ]( const Sentence& sentence )
                                  {
                                      return sentence.id == sentenceId;
                                  } );

    if ( it == sentences.end() )
    {
        throw Exception( "Unknown sentence type - " + sentenceId );
    }

    return *it;
}

} // namespace tools

std::list< rdbus::Data::Field > parse( const Response& response,
                                       const tools::Sentences& sentences,
                                       const tools::Timestamp& timestamp )
{
    using namespace tools;
    using namespace rdbus;
    using Fields = std::list< rdbus::Data::Field >;

    const auto& sentence = findSentence( sentences, response.getSentenceID() );

    if ( sentence.fields.size() != response.getFields().size() )
    {
        throw Exception( "Incoming sentence (" + response.getSentenceID() + ") and sentence size in config mismatch!" );
    }

    Fields output;

    auto sentenceIt = sentence.fields.begin();
    auto fieldIt = response.getFields().begin();
    for ( ; sentenceIt != sentence.fields.end() && fieldIt != response.getFields().end(); sentenceIt++, fieldIt++ )
    {
        rdbus::Data::Field field = {
            .name = sentenceIt->name,
            .timestamp = timestamp
        };

        if ( !fieldIt->empty() )
        {
            // Since we are parsing string fields to various different types, there is a possibility that
            // the string is not convertible to other types due to it containing unexpected characters
            // resulting in exception.
            try
            {
                switch ( sentenceIt->type )
                {
                    case Type::Int64:
                        field.value = std::stoll( *fieldIt );
                        break;
                    case Type::Uint64:
                        field.value = std::stoull( *fieldIt );
                        break;
                    case Type::Double:
                        field.value = std::stod( *fieldIt );
                        break;
                    case Type::String:
                        field.value = *fieldIt;
                        break;
                    case Type::None:
                        break;
                    default:
                        throw Exception( "Unsupported type " + std::to_string( static_cast< int >( sentenceIt->type ) ) + "!" );
                        break;
                }
            }
            catch ( const Exception& )
            {
                // This is known exception, rethrow it
                std::rethrow_exception( std::current_exception() );
            }
            catch ( const std::exception& e )
            {
                // The exception is not critical - make it rdbus exception
                throw Exception( e.what() );
            }
        }

        output.emplace_back( std::move( field ) );
    }

    return output;
}

std::list< std::vector< uint8_t > > split( const std::vector< uint8_t >& data )
{
    std::list< std::vector< uint8_t > > result;

    for ( const uint8_t c : data )
    {
        // if delimiter
        if ( c == '$' )
        {
            result.emplace_back( std::vector< uint8_t >{} );
        }

        // If there is a container to store data
        if ( !result.empty() )
        {
            result.back().emplace_back( c );
        }
    }

    return result;
}

} // namespace rdbus::communication::nmea::interpreter
