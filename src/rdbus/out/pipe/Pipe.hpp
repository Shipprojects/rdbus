#pragma once

#include "rdbus/out/Output.hpp"

namespace rdbus::out::pipe
{

class Pipe : public Output
{
public:
    void send( const std::list< rdbus::Data >& ) override;
    void send( const processing::Base::OutputList&, processing::Name ) override;
};

} // namespace rdbus::out::pipe
