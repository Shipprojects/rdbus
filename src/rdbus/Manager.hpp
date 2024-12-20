#pragma once

#include "out/Output.hpp"
#include "rdbus/processing/Processor.hpp"
#include "tasks/Task.hpp"
#include <list>
#include <memory>

namespace rdbus
{

// The task manager. Meant for tasks that operate on the same port.
class Manager
{
public:
    using Output = std::shared_ptr< out::Output >;
    using Tasks = std::list< std::unique_ptr< tasks::Task > >;
    using Processors = std::list< std::unique_ptr< processing::Processor > >;
    Manager( const std::string& name, Tasks, Output, Processors = {} );

    // Execute all tasks once
    void run();

    const std::string& getName() const;

private:
    std::string name; // The name of the manager
    Tasks tasks; // Tasks to execute
    Output output; // Output module
    Processors processors; // Optional data processors
};

} // namespace rdbus
