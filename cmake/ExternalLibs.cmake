function(add_spdlog TARGET_NAME)
    include(FetchContent)
    FetchContent_Declare(
            spdlog
            GIT_REPOSITORY  "https://github.com/gabime/spdlog"
            GIT_TAG         "v1.14.0"
    )

    FetchContent_MakeAvailable( spdlog )
    target_include_directories ( ${TARGET_NAME} PUBLIC ${spdlog_SOURCE_DIR}/include )
endfunction()


function(add_json TARGET_NAME)
    include(FetchContent)
    FetchContent_Declare(
            nlohmann
            GIT_REPOSITORY  "https://github.com/nlohmann/json"
            GIT_TAG         "v3.11.0"
    )

    FetchContent_MakeAvailable( nlohmann )
    target_include_directories ( ${TARGET_NAME} PUBLIC ${nlohmann_json_SOURCE_DIR}/include )
endfunction()

function(add_gtest TARGET_NAME)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/2f80c2ba71c0e8922a03b9b855e5b019ad1f7064.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    target_link_libraries(${TARGET_NAME} PRIVATE gtest_main)
    FetchContent_MakeAvailable(googletest)
endfunction()

