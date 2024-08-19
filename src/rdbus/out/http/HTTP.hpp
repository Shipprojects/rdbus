#pragma once

#include "Sessioner.hpp"
#include "rdbus/config/Output.hpp"
#include "rdbus/out/Output.hpp"
#include <chrono>
#include <mutex>
#include <nlohmann/json.hpp>
#include <thread>
#include <deque>

// Use 2 threads for server request processing
#define CPPHTTPLIB_THREAD_POOL_COUNT 2
#include <httplib.h>

namespace rdbus::out::http
{

class HTTP : public Output
{
public:
    HTTP( const config::Output& settings );
    ~HTTP();

    // This function does not actually send data but rather adds it to a buffer
    // which gets sent on client request
    void send( const std::list< rdbus::Data >& list ) override;

private:
    httplib::Server server;
    std::thread serverThread;
    std::mutex mutex;

    using TimePoint = std::chrono::time_point< std::chrono::system_clock >;
    using TimeDataPairs = std::deque< std::pair< TimePoint, rdbus::Data > >;
    TimeDataPairs buffer;
    void sizeConstrainBuffer();

    nlohmann::json parseFrom( const TimePoint& start );

    Sessioner sessioner;
};

} // namespace rdbus::out::http
