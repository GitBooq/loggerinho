# GoogleTest library
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(tests_all
    filters/level_filter_test.cc
    filters/name_filter_test.cc
    enrichers/timestamp_enricher_test.cc
    enrichers/enricher_factory_test.cc
    formatters/plain_text_formatter_test.cc
    formatters/json_formatter_test.cc
    sinks/console_sink_test.cc
    sinks/file_sink_test.cc
)

target_link_libraries(tests_all
    logger
    GTest::gtest_main
    GTest::gmock_main
)

target_include_directories(tests_all PRIVATE ${CMAKE_SOURCE_DIR}/tests)

target_compile_options(tests_all PRIVATE
    -O0
    -g
)

target_compile_features(tests_all PUBLIC cxx_std_20)

include(GoogleTest)
gtest_discover_tests(tests_all)
