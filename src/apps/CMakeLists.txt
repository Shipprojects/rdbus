set(TARGET_NAME ${CMAKE_PROJECT_NAME})

file(GLOB HEADER_FILES *.hpp)
file(GLOB SOURCE_FILES *.cpp)

add_executable(${TARGET_NAME}
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

add_compile_options(${COMPILE_OPTIONS})

configure_file(${CMAKE_SOURCE_DIR}/version.hpp.in ${CMAKE_CURRENT_SOURCE_DIR}/version.hpp)
target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                                  ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(${TARGET_NAME} PRIVATE lib_rdbus spdlog::spdlog)

add_json(${TARGET_NAME})
add_argparse(${TARGET_NAME})
add_spdlog(${TARGET_NAME})
add_http(${TARGET_NAME})
