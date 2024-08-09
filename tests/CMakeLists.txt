set(TARGET_NAME ${CMAKE_PROJECT_NAME}_tests)
set(TEST_DATA_DIR_HEADER_PATH ${CMAKE_BINARY_DIR}/tests/include)

file(GLOB_RECURSE HEADER_FILES *.hpp)
file(GLOB_RECURSE SOURCE_FILES *.cpp)

add_gtest(${TARGET_NAME})

add_executable(${TARGET_NAME}
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

add_compile_options(${TARGET_NAME} ${COMPILE_OPTIONS})

add_json(${TARGET_NAME})

configure_file(testDataDirectory.hpp.in  ${TEST_DATA_DIR_HEADER_PATH}/testDataDirectory.hpp)

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                                  ${gtest_SOURCE_DIR}/include
                                                  ${TEST_DATA_DIR_HEADER_PATH}
                                                  ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(${TARGET_NAME} PRIVATE lib_rdbus GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(${TARGET_NAME})
enable_testing()
