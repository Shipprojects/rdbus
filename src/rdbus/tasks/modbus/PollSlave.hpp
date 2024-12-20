#pragma once

#include "rdbus/tasks/Task.hpp"
#include "rdbus/communication/modbus/Communicator.hpp"
#include "rdbus/config/modbus/Slave.hpp"
#include <chrono>
#include <memory>

namespace rdbus::tasks::modbus
{

class PollSlave : public Task
{
private:
    const config::modbus::Slave slave;
    using Communicator = std::shared_ptr< rdbus::communication::modbus::Communicator >;
    Communicator com;
    std::chrono::time_point< std::chrono::steady_clock > lastRun;

public:
    PollSlave( const config::modbus::Slave& slave, const Communicator& communicator );
    std::list< Data > run() override;
};

} // namespace rdbus::tasks
