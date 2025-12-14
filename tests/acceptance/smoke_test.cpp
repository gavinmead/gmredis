#include <gmredis/version.h>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("GMRedis library is accessible", "[smoke]") {
    SECTION("Version information is available") {
        REQUIRE(gmredis::Version::major >= 0);
        REQUIRE(gmredis::Version::minor >= 0);
        REQUIRE(gmredis::Version::patch >= 0);
    }

    SECTION("Version string is not empty") {
        REQUIRE_FALSE(gmredis::Version::string().empty());
    }

    SECTION("Project name is gmredis") {
        REQUIRE(gmredis::Version::name() == "gmredis");
    }
}

TEST_CASE("Version info function works", "[smoke]") {
    const auto info = gmredis::get_version_info();

    SECTION("Returns non-empty string") {
        REQUIRE_FALSE(info.empty());
    }

    SECTION("Contains project name") {
        REQUIRE(info.find("gmredis") != std::string::npos);
    }

    SECTION("Contains version number") {
        REQUIRE(info.find("0.1.0") != std::string::npos);
    }
}
