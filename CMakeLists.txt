cmake_minimum_required(VERSION 3.15...4.1)

project(
    Logger
    VERSION 0.1.0
    DESCRIPTION "C++ Logger Library"
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

option(ENABLE_SANITIZERS "Enable sanitizers" OFF)
option(BUILD_EXAMPLE "Build example" OFF)
option(BUILD_SHARED_LIBS "Build shared library" ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
    )
endif()

include(CTest)
include(FetchContent)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(ENABLE_SANITIZERS)
    add_compile_options(
        -fsanitize=address,undefined
        -g
        -fno-omit-frame-pointer
    )

    add_link_options(
        -fsanitize=address,undefined
    )
endif()

# ----------------------------------------------------------
# Dependencies
# ----------------------------------------------------------

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(json)

# ----------------------------------------------------------
# Library
# ----------------------------------------------------------

add_library(logger
    src/sinks/file_sink.cc
    src/sinks/buffered_sink.cc
    src/sinks/console_sink.cc
    src/formatters/json_formatter.cc
)

add_library(Logger::logger ALIAS logger)

target_compile_features(logger PUBLIC cxx_std_20)

target_include_directories(logger
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/logger>
)

target_link_libraries(logger
    PRIVATE
        nlohmann_json::nlohmann_json
)

# ----------------------------------------------------------
# Installation
# ----------------------------------------------------------

install(
    TARGETS logger
    EXPORT LoggerTargets

    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}

    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/logger
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/logger
)

# ----------------------------------------------------------
# Package config
# ----------------------------------------------------------

install(
    EXPORT LoggerTargets
    FILE LoggerTargets.cmake
    NAMESPACE Logger::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Logger
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/LoggerConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/LoggerConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Logger
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/LoggerConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/LoggerConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/LoggerConfigVersion.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/Logger
)

# ----------------------------------------------------------
# Optional targets
# ----------------------------------------------------------

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif()