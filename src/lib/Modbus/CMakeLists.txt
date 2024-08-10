cmake_minimum_required(VERSION 3.13)
project(protocolConverter)

set(CMAKE_CXX_STANDARD 17)

if (DCMAKE_BUILD_TYPE MATCHES Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -lpthread -Wall")
endif()

add_subdirectory(src)
