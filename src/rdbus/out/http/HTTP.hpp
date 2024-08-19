#pragma once

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

class HTTP : public Output
{
public:
    HTTP( const config::Output& settings );
    void send( const std::list< rdbus::Data >& list ) override;
    ~HTTP();

private:
    httplib::Server server;

    nlohmann::json parse();
    std::vector< rdbus::Data > buffer;

    std::thread serverThread;
    std::mutex mutex;

    void sizeConstrainBuffer();
};

} // namespace rdbus::out::http
