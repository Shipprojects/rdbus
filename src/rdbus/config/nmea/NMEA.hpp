#pragma once

#include "Sentence.hpp"
#include <list>
#include <string>

namespace rdbus::config::nmea
{

struct NMEA
{
    using Sentences = std::list< Sentence >;
    Sentences sentences;

    std::string talkerId;
    bool withChecksum = false;
};

} // namespace rdbus::config::nmea
