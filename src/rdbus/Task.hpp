#pragma once

#include "Data.hpp"
#include <list>

namespace rdbus
{

class Task
{
public:
    virtual std::list< Data > run() = 0;
    virtual ~Task() = default;
};

} // namespace rdbus
