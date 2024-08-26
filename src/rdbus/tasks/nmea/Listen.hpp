#pragma once

#include "rdbus/communication/nmea/Communicator.hpp"
#include "rdbus/tasks/Task.hpp"
#include <memory>

namespace rdbus::tasks::nmea
{

class Listen : public Task
{
private:
    const config::nmea::NMEA nmea;
    using Communicator = std::shared_ptr< rdbus::communication::nmea::Communicator >;
    Communicator com;

public:
    Listen( const config::nmea::NMEA& nmea, const Communicator& communicator );
    std::list< Data > run() override;
};

} // namespace rdbus::tasks::nmea
