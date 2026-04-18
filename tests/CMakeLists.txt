cmake_minimum_required(VERSION 3.28)
project(crow_test_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies: Crow and Asio via FetchContent, nlohmann_json via Conan (if available)
include(FetchContent)

# Try to find nlohmann_json from Conan first, fallback to FetchContent if not found
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG        v3.11.3
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG        asio-1-30-2
)
FetchContent_MakeAvailable(asio)
set(ASIO_INCLUDE_DIR ${asio_SOURCE_DIR}/asio/include)

FetchContent_Declare(
    crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.2.0
)
FetchContent_MakeAvailable(crow)

add_executable(crow_test_server
    src/main.cpp
)

target_include_directories(crow_test_server
    PRIVATE
        ${crow_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/../include
    ${ASIO_INCLUDE_DIR}
)

target_compile_features(crow_test_server PUBLIC cxx_std_23)

target_link_libraries(crow_test_server PRIVATE
    Crow::Crow
    nlohmann_json::nlohmann_json
)

file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/libs_test.txt" 
     CONTENT "$<TARGET_PROPERTY:crow_test_server,LINK_LIBRARIES>")
