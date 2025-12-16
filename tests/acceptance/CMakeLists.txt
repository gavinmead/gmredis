add_executable(gmredis_acceptance_tests
    smoke_test.cpp
    parser_integration_test.cpp
)

target_link_libraries(gmredis_acceptance_tests
    PRIVATE
        gmredis::lib
        Catch2::Catch2WithMain
        gmredis_warnings
)

include(Catch)
catch_discover_tests(gmredis_acceptance_tests
    PROPERTIES LABELS "acceptance"
)
