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

function(add_modbus TARGET_NAME)
    # include(FetchContent)
    # FetchContent_Declare(
    #         modbus
    #         GIT_REPOSITORY  "https://github.com/Mazurel/Modbus"
    #         GIT_TAG         "master"
    # )

    # set( MODBUS_COMMUNICATION off )
    # FetchContent_MakeAvailable( modbus )
    target_include_directories ( ${TARGET_NAME} PUBLIC ${protocolConverter_SOURCE_DIR}/include )
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
        URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
    )
    FetchContent_MakeAvailable(googletest)
endfunction()

