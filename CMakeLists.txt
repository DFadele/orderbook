set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

# Require a modern CMake
cmake_minimum_required(VERSION 3.16)

# Project name and language
project(orderbook LANGUAGES CXX)

# Use C++20 features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Show all warnings (GCC/Clang)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif (MSVC)
    add_compile_options(/W4 /permissive-)
endif()

# Find dependencies installed via vcpkg
find_package(ixwebsocket CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Source files 
add_executable(orderbook
    src/main.cpp
    src/utils/logger.cpp
)

target_include_directories(orderbook PRIVATE include)

# Link dependencies to the target
target_link_libraries(orderbook
    PRIVATE
        ixwebsocket::ixwebsocket
        OpenSSL::SSL
        OpenSSL::Crypto
        nlohmann_json::nlohmann_json
)

# Set output directory so binary is easy to find
set_target_properties(orderbook PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
