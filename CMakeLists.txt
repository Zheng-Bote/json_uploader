cmake_minimum_required(VERSION 3.28)
project(json_uploader 
VERSION 1.0.0 
DESCRIPTION "JSON Uploader CLI Application"
HOMEPAGE_URL "https://github.com/Zheng-Bote/json_uploader"
LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler flags
if(MSVC)
    add_compile_options(/std:c++latest /W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow -Wconversion)
endif()

# Dependencies
include(FetchContent)

find_package(PkgConfig REQUIRED)
find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(simdjson REQUIRED)
pkg_check_modules(ZSTD REQUIRED IMPORTED_TARGET libzstd)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
# Valijson is header-only
find_package(valijson QUIET) 

FetchContent_Declare(
    dotenv-cpp
    GIT_REPOSITORY https://github.com/laserpants/dotenv-cpp.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(dotenv-cpp)

# libsodium - official repo has no CMakeLists.txt
include(ExternalProject)
FetchContent_Declare(
    libsodium_src
    GIT_REPOSITORY https://github.com/jedisct1/libsodium.git
    GIT_TAG        stable
)
FetchContent_GetProperties(libsodium_src)
if(NOT libsodium_src_POPULATED)
    FetchContent_Populate(libsodium_src)
    
    if(UNIX)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/libsodium_install/include)
        ExternalProject_Add(libsodium_project
            SOURCE_DIR ${libsodium_src_SOURCE_DIR}
            CONFIGURE_COMMAND ${libsodium_src_SOURCE_DIR}/configure --prefix=${CMAKE_BINARY_DIR}/libsodium_install --disable-shared --with-pic
            BUILD_COMMAND make -j4
            INSTALL_COMMAND make install
            BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/libsodium_install/lib/libsodium.a
        )
        add_library(sodium STATIC IMPORTED GLOBAL)
        set_target_properties(sodium PROPERTIES
            IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/libsodium_install/lib/libsodium.a
            INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_BINARY_DIR}/libsodium_install/include
        )
        add_dependencies(sodium libsodium_project)
    else()
        # Fallback/Placeholder for Windows or other systems
        message(WARNING "libsodium build not fully implemented for this platform in this script. Using find_package fallback.")
        find_package(sodium REQUIRED)
    endif()
endif()

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
    ZLIB::ZLIB
    simdjson::simdjson
    PkgConfig::ZSTD
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    dotenv
    sodium
)


file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/libs.txt" 
     CONTENT "$<TARGET_PROPERTY:${PROJECT_NAME},LINK_LIBRARIES>")
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/include" 
     CONTENT "$<TARGET_PROPERTY:${PROJECT_NAME},INCLUDE_DIRECTORIES>")
