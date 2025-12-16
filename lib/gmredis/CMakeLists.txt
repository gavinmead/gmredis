add_library(gmredis_lib
        src/version.cpp
        src/storage/kv_mem.cpp
        src/storage/kv_threading.cpp
        src/protocol/serialize.cpp
)

add_library(gmredis::lib ALIAS gmredis_lib)

target_include_directories(gmredis_lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(gmredis_lib
    PRIVATE
        gmredis_warnings
    PUBLIC
        spdlog::spdlog
        Threads::Threads
)

# Set library properties
set_target_properties(gmredis_lib PROPERTIES
    OUTPUT_NAME gmredis
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)
