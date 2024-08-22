#include "Interpreter.hpp"
#include "rdbus/Data.hpp"
#include <stdexcept>

namespace rdbus::communication::nmea::interpreter
{

namespace tools
{

const Sentence& findSentence( const tools::Sentences& sentences, const std::string& sentenceId )
{
    const auto it = std::find_if( sentences.begin(), sentences.end(),
                                  [ & ]( const Sentence& sentence )
                                  {
                                      return sentence.id == sentenceId;
                                  } );

    if ( it == sentences.end() )
    {
        throw std::runtime_error( "Unknown sentence type -" + sentenceId );
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

    assert( sentence.fields.size() == response.getFields().size() );

    Fields output;

    for ( const auto& sentenceField : sentence.fields )
    {
        const int i = output.size();

        rdbus::Data::Field field = {
            .name = sentenceField.name,
            .type = sentenceField.type,
            .timestamp = timestamp
        };

        // Indicate that field is empty
        if ( response.getFields()[ i ].empty() )
        {
            field.type = rdbus::Type::None;
        }

        switch ( field.type )
        {
            case Type::Int64:
                field.value = std::stoll( response.getFields()[ i ] );
                break;
            case Type::Uint64:
                field.value = std::stoull( response.getFields()[ i ] );
                break;
            case Type::Double:
                field.value = std::stod( response.getFields()[ i ] );
                break;
            case Type::String:
                field.value = response.getFields()[ i ];
                break;
            case Type::None:
                break;
            default:
                throw std::invalid_argument( "Unsupported type " + std::to_string( static_cast< int >( field.type ) ) + "!" );
                break;
        }

        output.emplace_back( std::move( field ) );
    }

    return output;
}

} // namespace rdbus::communication::nmea::interpreter
