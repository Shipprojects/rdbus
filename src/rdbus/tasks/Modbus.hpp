#pragma once

#include "rdbus/Task.hpp"
#include "rdbus/communication/modbus/Communicator.hpp"
#include "rdbus/config/Slave.hpp"
#include <chrono>

namespace rdbus::tasks
{

class Modbus : public Task
{
public:
    Modbus( const config::Slave& slave, rdbus::communication::modbus::Communicator& communicator );
    std::list< Data > run() override;

private:
    const config::Slave slave;
    rdbus::communication::modbus::Communicator& com;
    std::chrono::time_point< std::chrono::steady_clock > lastRun;
};

} // namespace rdbus::tasks
