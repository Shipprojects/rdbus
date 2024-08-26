#pragma once

#include "out/Output.hpp"
#include "rdbus/Data.hpp"
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
    Manager( const std::string& name, Tasks tasks, Output output );

    // Execute all tasks once
    void run();

    const std::string& getName() const;

private:
    std::string name;
    Tasks tasks;
    Output output;
    std::list< Data > run( std::unique_ptr< tasks::Task >& task );
    void send( const std::list< rdbus::Data >& list );
};

} // namespace rdbus
