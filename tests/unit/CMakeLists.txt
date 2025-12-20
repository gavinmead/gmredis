add_executable(gmredis_unit_tests
    version_test.cpp
    storage/kv_mem_test.cpp
    storage/kv_threaded_test.cpp
    protocol/serialize_test.cpp
    protocol/parse_test.cpp
    command/command_test.cpp
    command/base_command_test.cpp
    command/command_registry_test.cpp
    command/ping_test.cpp
)

target_link_libraries(gmredis_unit_tests
    PRIVATE
        gmredis::lib
        GTest::gtest
        GTest::gtest_main
        gmredis_warnings
)

# Access to library internals for testing
target_include_directories(gmredis_unit_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/gmredis/src
)

include(GoogleTest)
gtest_discover_tests(gmredis_unit_tests
    PROPERTIES LABELS "unit"
)
