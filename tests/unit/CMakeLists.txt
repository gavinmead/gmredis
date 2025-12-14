add_executable(gmredis_unit_tests
    version_test.cpp
)

target_link_libraries(gmredis_unit_tests
    PRIVATE
        gmredis::lib
        GTest::gtest
        GTest::gtest_main
        gmredis_warnings
)

include(GoogleTest)
gtest_discover_tests(gmredis_unit_tests
    PROPERTIES LABELS "unit"
)
