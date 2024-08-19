#pragma once

#include "rdbus/out/Output.hpp"

namespace rdbus::out::pipe
{

class Pipe : public Output
{
public:
    Pipe() = default;
    void send( const std::list< rdbus::Data >& list ) override;

    ~Pipe() = default;

private:
    void send( const rdbus::Data& data );
};

} // namespace rdbus::out::pipe
