#pragma once

#include "rdbus/out/Output.hpp"

namespace rdbus::out::pipe
{

class Pipe : public Output
{
public:
    Pipe();

    void send( const std::list< rdbus::Data >& list ) override;
    void send( const rdbus::Data& data ) override;

    ~Pipe() = default;
};

} // namespace rdbus::out::pipe
