#pragma once

#include "Task.hpp"
#include "rdbus/communication/modbus/Communicator.hpp"
#include "rdbus/config/Slave.hpp"
#include <chrono>
#include <memory>

namespace rdbus::tasks
{

class Modbus : public Task
{
private:
    const config::Slave slave;
    using Communicator = std::shared_ptr< rdbus::communication::modbus::Communicator >;
    Communicator com;
    std::chrono::time_point< std::chrono::steady_clock > lastRun;

public:
    Modbus( const config::Slave& slave, const Communicator& communicator );
    std::optional< Data > run() override;
};

} // namespace rdbus::tasks
