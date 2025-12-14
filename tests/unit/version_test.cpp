#include <gmredis/version.h>

#include <gtest/gtest.h>

namespace gmredis::test {

TEST(VersionTest, MajorVersionIsZero) {
    EXPECT_EQ(Version::major, 0);
}

TEST(VersionTest, MinorVersionIsOne) {
    EXPECT_EQ(Version::minor, 1);
}

TEST(VersionTest, PatchVersionIsZero) {
    EXPECT_EQ(Version::patch, 0);
}

TEST(VersionTest, VersionStringIsCorrect) {
    EXPECT_EQ(Version::string(), "0.1.0");
}

TEST(VersionTest, NameIsGmredis) {
    EXPECT_EQ(Version::name(), "gmredis");
}

TEST(VersionTest, GetVersionInfoContainsName) {
    const auto info = get_version_info();
    EXPECT_NE(info.find("gmredis"), std::string::npos);
}

TEST(VersionTest, GetVersionInfoContainsVersion) {
    const auto info = get_version_info();
    EXPECT_NE(info.find("0.1.0"), std::string::npos);
}

}  // namespace gmredis::test
