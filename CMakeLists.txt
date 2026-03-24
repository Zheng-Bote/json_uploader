cmake_minimum_required(VERSION 3.28)
project(json_uploader VERSION 0.2.8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
if(MSVC)
    add_compile_options(/std:c++latest /W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion)
endif()

# Dependencies
find_package(PkgConfig REQUIRED)
find_package(CURL REQUIRED)
find_package(simdjson REQUIRED)
pkg_check_modules(ZSTD REQUIRED IMPORTED_TARGET libzstd)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
# Valijson is header-only
find_package(valijson QUIET) 

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SOURCES
    src/main.cpp
    src/cli_parser_ctrl.cpp
    include/cli_parser_ctrl.hpp
    src/env_util.cpp
    include/env_util.hpp
    src/logger_util.cpp
    include/logger_util.hpp
    src/auth_srv.cpp
    include/auth_srv.hpp
    src/uploader_srv.cpp
    include/uploader_srv.hpp
    src/validator_util.cpp
    include/validator_util.hpp
    src/email_srv.cpp
    include/email_srv.hpp
    include/error_type.hpp
    include/config_type.hpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    CURL::libcurl
    simdjson::simdjson
    PkgConfig::ZSTD
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
