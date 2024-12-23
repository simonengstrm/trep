cmake_minimum_required(VERSION 3.15)

# Set the project name
project(trep)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set cmake binary directory to build
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Specify clang++ as the compiler
set(CMAKE_CXX_COMPILER clang++)

# Set flags
add_compile_options(-Wno-c23-extensions)

# Inclue boost library
find_package(Boost CONFIG REQUIRED COMPONENTS regex)

# Make clangd find compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Make clangd find boost regex
include_directories(${Boost_INCLUDE_DIRS})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
endif()

# Set release flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Add the executable
# Add all sources to the SOURCES variable
file(GLOB SOURCES "src/*.cxx" "src/*.hxx")
add_executable(trep ${SOURCES})
target_link_libraries(trep PRIVATE Boost::regex)
