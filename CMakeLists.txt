cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

file(READ VERSION CURRENT_VERSION)
project(rdbus VERSION ${CURRENT_VERSION})
message(STATUS "${PROJECT_NAME} ${CURRENT_VERSION}")

include(CTest)
include(ExternalLibs)

set(COMPILE_OPTIONS -Wall -Wextra -Wpedantic)

add_subdirectory(src/rdbus)
add_subdirectory(src/apps)
add_subdirectory(tests)
