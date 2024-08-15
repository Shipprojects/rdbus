#pragma once

#include "rdbus/Data.hpp"
#include <optional>

namespace rdbus::tasks
{

class Task
{
public:
    virtual std::optional< Data > run() = 0;
    virtual ~Task() = default;
};

} // namespace rdbus::tasks
