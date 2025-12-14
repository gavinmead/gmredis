add_executable(gmredis-cli
    main.cpp
)

target_link_libraries(gmredis-cli
    PRIVATE
        gmredis::lib
        gmredis_warnings
)

set_target_properties(gmredis-cli PROPERTIES
    OUTPUT_NAME gmredis-cli
)
