cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(rdbus)

include(CTest)
include(ExternalLibs)

set(COMPILE_OPTIONS -Wall -Wextra -Wpedantic)

add_subdirectory(src/rdbus)
add_subdirectory(src/apps)
add_subdirectory(tests)

# TODO: For some reason, fetched Modbus library does not build it's target. Investigate and fix.
add_subdirectory(src/lib/Modbus)
