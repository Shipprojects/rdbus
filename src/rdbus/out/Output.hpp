#pragma once

#include "rdbus/Data.hpp"
#include "rdbus/processing/Base.hpp"

namespace rdbus::out
{

class Output
{
public:
    virtual void send( const std::list< rdbus::Data >& ) = 0;
    virtual void send( const processing::Base::OutputList&, processing::Name ) = 0;
    virtual ~Output() = default;
};

} // namespace rdbus::out
