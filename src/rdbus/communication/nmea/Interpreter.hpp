#pragma once

#include "Response.hpp"
#include "rdbus/Data.hpp"
#include "rdbus/communication/Interpreter.hpp"
#include "rdbus/config/nmea/NMEA.hpp"
#include <stdint.h>
#include <vector>

namespace rdbus::communication::nmea::interpreter
{

namespace tools
{

struct Exception : rdbus::communication::interpreter::Exception
{
    Exception( const std::string& what );
};

using Sentences = rdbus::config::nmea::NMEA::Sentences;
using Timestamp = rdbus::Data::Field::Timestamp;

using Sentence = rdbus::config::nmea::Sentence;
// Find sentence from configuration by using given sentenceId
const Sentence& findSentenceType( const tools::Sentences& sentences, const std::string& sentenceId );

} // namespace tools

// Splits a single blob of data into multiple chunks by NMEA '$' delimiter
std::list< std::vector< uint8_t > > split( const std::vector< uint8_t >& data );

std::list< rdbus::Data::Field > parse( const Response&,
                                       const tools::Sentences&,
                                       const tools::Timestamp& );

} // namespace rdbus::communication::nmea::interpreter
