#pragma once

#include "rdbus/Exception.hpp"
#include "rdbus/config/Address.hpp"
#include <SessionInfo.h>
#include <cip/EPath.h>
#include <memory>

namespace rdbus::communication
{

// Interface of Ethernet Industrial Protocol
class EIP
{
public:
    explicit EIP( const config::Address& );

    int16_t getSingle( const eipScanner::cip::EPath&, eipScanner::cip::CipUsint serviceId );

    struct Exception : public rdbus::Exception
    {
        explicit Exception( const std::string& what );
    };

private:
    std::shared_ptr< eipScanner::SessionInfo > session;
};

} // namespace rdbus::communication
