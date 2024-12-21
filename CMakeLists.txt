cmake_minimum_required(VERSION 3.10)

# Set the project name
project(trep)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set cmake binary directory to build
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Specify clang++ as the compiler
set(CMAKE_CXX_COMPILER clang++)

# Set flags
add_compile_options(-Wno-c23-extensions)


# Add the executable
set(SOURCES main.cxx colors.hxx)
add_executable(trep main.cxx)