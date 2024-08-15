#pragma once

#include "out/Output.hpp"
#include "rdbus/Data.hpp"
#include "tasks/Task.hpp"
#include <list>
#include <memory>

namespace rdbus
{

class Manager
{
public:
    using Output = std::unique_ptr< out::Output >;
    using Tasks = std::list< std::unique_ptr< tasks::Task > >;
    Manager( Tasks tasks, Output output );

    void run();

private:
    Tasks tasks;
    Output output;
    std::optional< Data > run( std::unique_ptr< tasks::Task >& task );
};

} // namespace rdbus
