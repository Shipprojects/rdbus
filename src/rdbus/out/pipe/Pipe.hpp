#pragma once

#include "rdbus/out/Output.hpp"

namespace rdbus::out::pipe
{

class Pipe : public Output
{
public:
    void send( const std::list< rdbus::Data >& list ) override;
};

} // namespace rdbus::out::pipe
