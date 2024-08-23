#pragma once

#include <stdexcept>
#include <cstdint>
#include <string>
#include <vector>

namespace rdbus::communication::nmea
{

// A basic representation of uninterpreted NMEA response
class Response
{
public:
    Response( const std::vector< uint8_t >& data, bool validateChecksum = false );

    struct Exception : public std::runtime_error
    {
        explicit Exception( const std::string& what );
    };

    const std::string& getTalkerID() const;
    const std::string& getSentenceID() const;

    using Fields = std::vector< std::string >;
    const Fields& getFields() const;

private:
    std::string talkerID;
    std::string sentenceId;

    Fields fields;
};

} // namespace rdbus::communication::nmea
