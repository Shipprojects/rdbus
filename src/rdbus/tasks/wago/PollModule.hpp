#pragma once

#include "rdbus/communication/wago/Communicator.hpp"
#include "rdbus/config/wago/Module.hpp"
#include "rdbus/tasks/Task.hpp"
#include <chrono>
#include <memory>

namespace rdbus::tasks::wago
{

class PollModule : public Task
{
private:
    const config::wago::Module module;
    using Communicator = std::shared_ptr< rdbus::communication::wago::Communicator >;
    Communicator com;
    std::chrono::time_point< std::chrono::steady_clock > lastRun;

public:
    PollModule( const config::wago::Module& module, const Communicator& communicator );
    std::list< Data > run() override;
};

} // namespace rdbus::tasks::modbus
