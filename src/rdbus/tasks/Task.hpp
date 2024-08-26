#pragma once

#include "rdbus/Data.hpp"
#include <list>

namespace rdbus::tasks
{

class Task
{
public:
    virtual std::list< Data > run() = 0;
    virtual ~Task() = default;
};

} // namespace rdbus::tasks
