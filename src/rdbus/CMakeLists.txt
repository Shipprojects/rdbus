set(TARGET_NAME lib_${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE HEADER_FILES *.hpp)
file(GLOB_RECURSE SOURCE_FILES *.cpp)

add_library(${TARGET_NAME}
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

add_compile_options(${COMPILE_OPTIONS})

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(${TARGET_NAME} PRIVATE Modbus spdlog::spdlog EIPScannerS)

add_json(${TARGET_NAME})
add_spdlog(${TARGET_NAME})
add_modbus(${TARGET_NAME})
add_http(${TARGET_NAME})
add_eipscanner(${TARGET_NAME})
