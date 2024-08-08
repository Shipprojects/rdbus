set(TARGET_NAME lib_${CMAKE_PROJECT_NAME})

file(GLOB HEADER_FILES *.hpp)
file(GLOB SOURCE_FILES *.cpp)

add_library(${TARGET_NAME}
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
