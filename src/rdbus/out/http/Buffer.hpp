#pragma once

#include "rdbus/Data.hpp"
#include <deque>
#include <nlohmann/json.hpp>

namespace rdbus::out::http
{

class Buffer
{
public:
    void add( const std::list< rdbus::Data >& list );

    using TimePoint = std::chrono::time_point< std::chrono::system_clock >;
    nlohmann::json parseFrom( const TimePoint& lastTime );

    using TimeDataPairs = std::deque< std::pair< TimePoint, rdbus::Data > >;
    TimeDataPairs data;
    constexpr static auto maxBytes = 500'000;

private:
    void constrainSize();
};

} // namespace rdbus::out::http
