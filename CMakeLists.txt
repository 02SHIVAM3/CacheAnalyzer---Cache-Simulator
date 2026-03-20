# Minimum required version for CMake. Using 3.15+ is generally safe for modern projects.
cmake_minimum_required(VERSION 3.15)

# Project Definition
project(CacheIQ
    VERSION 1.0.0
    LANGUAGES CXX
)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configure the build directory for a clean separation of files
# This ensures executables and libraries go into standard locations
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Add common compiler flags for better warnings/debugging
if (NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

# Add the main application source directory
add_subdirectory(src)