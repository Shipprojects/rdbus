#pragma once

#include "Buffer.hpp"
#include "Sessioner.hpp"
#include "rdbus/config/Output.hpp"
#include "rdbus/out/Output.hpp"
#include <mutex>
#include <nlohmann/json.hpp>
#include <thread>

// Use 2 threads for server request processing
#define CPPHTTPLIB_THREAD_POOL_COUNT 2
#include <httplib.h>

namespace rdbus::out::http
{

// An HTTP server under the hood
class HTTP : public Output
{
public:
    explicit HTTP( const config::Output& settings );
    ~HTTP();

    // This function does not actually send data but rather adds it to a buffer
    // which gets sent on client request
    void send( const std::list< rdbus::Data >& list ) override;

private:
    httplib::Server server;
    std::thread serverThread;
    std::mutex mutex;

    Buffer buffer;
    Sessioner sessioner;
};

} // namespace rdbus::out::http
