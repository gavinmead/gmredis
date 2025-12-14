add_executable(gmredis-server
    main.cpp
)

target_link_libraries(gmredis-server
    PRIVATE
        gmredis::lib
        gmredis_warnings
)

set_target_properties(gmredis-server PROPERTIES
    OUTPUT_NAME gmredis-server
)
