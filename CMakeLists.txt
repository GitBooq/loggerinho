cmake_minimum_required(VERSION 3.15...4.1)

project(
    Logger
    VERSION 0.1
    DESCRIPTION "C++ Logger Library"
    LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Let's ensure -std=c++xx instead of -std=g++xx
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

option(ENABLE_SANITIZERS "Enable sanitizers" OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
    # exclude self assignment as error
    # add_compile_options(-Wno-error=self-assign-overloaded)
endif()

include(CTest)
include(FetchContent)

if (ENABLE_SANITIZERS)
    add_compile_options(-fsanitize=address,undefined -g -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

# json library
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(json)

add_library(logger 
    src/sinks/file_sink.cc
    src/sinks/buffered_sink.cc
    src/sinks/console_sink.cc
)

target_include_directories(logger 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/core)

target_compile_features(logger PUBLIC cxx_std_20)

target_link_libraries(logger PUBLIC nlohmann_json::nlohmann_json)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

add_subdirectory(example)