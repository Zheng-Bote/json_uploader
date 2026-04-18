cmake_minimum_required(VERSION 3.28)
project(json_uploader 
VERSION 1.2.0 
DESCRIPTION "JSON Uploader CLI Application"
HOMEPAGE_URL "https://github.com/Zheng-Bote/json_uploader"
LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Configure (für rz_config.hpp) ---
set(EXECUTABLE_NAME "${PROJECT_NAME}")
set(PROG_LONGNAME "JSON Uploader CLI Application")
set(PROG_CREATED "2026")
set(PROG_AUTHOR "ZHENG Bote")
set(PROG_ORGANIZATION_NAME "ZHENG Robert")
set(PROG_ORGANIZATION_DOMAIN "net.hase-zheng")
# create rz_config.hpp
add_subdirectory(configure) 

# Compiler flags
if(MSVC)
    add_compile_options(/std:c++latest /W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion)
endif()

# Dependencies (Conan v2 - Static)
find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(simdjson REQUIRED)
find_package(zstd REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
find_package(valijson REQUIRED)
find_package(libsodium REQUIRED)

include(FetchContent)

# dotenv-cpp remains as FetchContent
FetchContent_Declare(
    dotenv-cpp
    GIT_REPOSITORY https://github.com/laserpants/dotenv-cpp.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(dotenv-cpp)

FetchContent_Declare(
  gh_update_checker
  GIT_REPOSITORY https://github.com/Zheng-Bote/gh-update-checker.git
  GIT_TAG        v1.0.8
)

FetchContent_MakeAvailable(gh_update_checker)
# Ensure gh_update_checker::gh_update_checker is available (for both FetchContent and find_package)
if(TARGET gh_update_checker AND NOT TARGET gh_update_checker::gh_update_checker)
    add_library(gh_update_checker::gh_update_checker ALIAS gh_update_checker)
endif()


# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Valijson often provides valijson::valijson or Valijson::Valijson, 
# but as a header-only library, we'll ensure we have access via its include directory if the target is missing.
if(NOT TARGET valijson::valijson)
    if(valijson_INCLUDE_DIRS)
        add_library(valijson::valijson INTERFACE IMPORTED)
        set_target_properties(valijson::valijson PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${valijson_INCLUDE_DIRS}"
        )
    elseif(TARGET valijson)
        add_library(valijson::valijson ALIAS valijson)
    endif()
endif()

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

# Target linking focusing on static Conan targets
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    CURL::libcurl
    ZLIB::ZLIB
    simdjson::simdjson
    $<IF:$<TARGET_EXISTS:zstd::libzstd_static>,zstd::libzstd_static,zstd::libzstd_shared>
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    $<IF:$<TARGET_EXISTS:ValiJSON::valijson>,ValiJSON::valijson,valijson::valijson>
    $<IF:$<TARGET_EXISTS:libsodium::libsodium>,libsodium::libsodium,sodium>
    dotenv
    gh_update_checker::gh_update_checker
)

# Standard metadata generation
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/project_libs.txt" 
     CONTENT "$<TARGET_PROPERTY:${PROJECT_NAME},LINK_LIBRARIES>")
file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/include_paths.txt" 
     CONTENT "$<TARGET_PROPERTY:${PROJECT_NAME},INCLUDE_DIRECTORIES>")
