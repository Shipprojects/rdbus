#pragma once

#include "rdbus/tasks/Task.hpp"
#include "rdbus/communication/modbus/Communicator.hpp"
#include "rdbus/config/Slave.hpp"
#include <chrono>
#include <memory>

namespace rdbus::tasks::modbus
{

class PollSlave : public Task
{
private:
    const config::Slave slave;
    using Communicator = std::shared_ptr< rdbus::communication::modbus::Communicator >;
    Communicator com;
    std::chrono::time_point< std::chrono::steady_clock > lastRun;

public:
    PollSlave( const config::Slave& slave, const Communicator& communicator );
    std::optional< Data > run() override;
};

} // namespace rdbus::tasks
