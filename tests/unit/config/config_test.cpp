#include <gmredis/config/config.h>

#include <gtest/gtest.h>

namespace gmredis::config::test {
    // Valid ServerConfiguration creation tests
    TEST(ServerConfigurationTest, CreateWithDefaultConfiguration) {
        auto config = ServerConfiguration::createServerConfiguration();

        ASSERT_TRUE(config.has_value());
        EXPECT_EQ(config->getHost(), "0.0.0.0");
        EXPECT_EQ(config->getPort(), 6379);
    }

    TEST(ServerConfigurationTest, CreateWithValidHostAndPort) {
        auto config = ServerConfiguration::createServerConfiguration("localhost", 6379);

        ASSERT_TRUE(config.has_value());
        EXPECT_EQ(config->getHost(), "localhost");
        EXPECT_EQ(config->getPort(), 6379);
    }

    TEST(ServerConfigurationTest, CreateWithIPAddress) {
        auto config = ServerConfiguration::createServerConfiguration("127.0.0.1", 6379);

        ASSERT_TRUE(config.has_value());
        EXPECT_EQ(config->getHost(), "127.0.0.1");
        EXPECT_EQ(config->getPort(), 6379);
    }

    TEST(ServerConfigurationTest, CreateWithDomainName) {
        auto config = ServerConfiguration::createServerConfiguration("redis.example.com", 6379);

        ASSERT_TRUE(config.has_value());
        EXPECT_EQ(config->getHost(), "redis.example.com");
        EXPECT_EQ(config->getPort(), 6379);
    }

    TEST(ServerConfigurationTest, CreateWithMinimumValidPort) {
        auto config = ServerConfiguration::createServerConfiguration("localhost", 1);

        ASSERT_TRUE(config.has_value());
        EXPECT_EQ(config->getPort(), 1);
    }

    TEST(ServerConfigurationTest, CreateWithMaximumValidPort) {
        auto config = ServerConfiguration::createServerConfiguration("localhost", 65535);

        ASSERT_TRUE(config.has_value());
        EXPECT_EQ(config->getPort(), 65535);
    }

    // Invalid ServerConfiguration creation tests
    TEST(ServerConfigurationTest, CreateWithEmptyHostReturnsError) {
        auto config = ServerConfiguration::createServerConfiguration("", 6379);

        ASSERT_FALSE(config.has_value());
        EXPECT_EQ(config.error().errorCode, ConfigurationErrorCode::InvalidConfiguration);
        EXPECT_FALSE(config.error().message.empty());
    }

    TEST(ServerConfigurationTest, CreateWithZeroPortReturnsError) {
        auto config = ServerConfiguration::createServerConfiguration("localhost", 0);

        ASSERT_FALSE(config.has_value());
        EXPECT_EQ(config.error().errorCode, ConfigurationErrorCode::InvalidConfiguration);
        EXPECT_FALSE(config.error().message.empty());
    }

    TEST(ServerConfigurationTest, CreateWithEmptyspaceOnlyHostReturnsError) {
        auto config = ServerConfiguration::createServerConfiguration("   ", 6379);

        ASSERT_FALSE(config.has_value());
        EXPECT_EQ(config.error().errorCode, ConfigurationErrorCode::InvalidConfiguration);
        EXPECT_FALSE(config.error().message.empty());
    }

    TEST(ServerConfigurationTest, CreateWithEmptyHostAndZeroPortReturnsError) {
        auto config = ServerConfiguration::createServerConfiguration("", 0);

        ASSERT_FALSE(config.has_value());
        EXPECT_EQ(config.error().errorCode, ConfigurationErrorCode::InvalidConfiguration);
        EXPECT_FALSE(config.error().message.empty());
    }

    TEST(ServerConfigurationTest, CreateWithNegativePortReturnsError) {
        auto config = ServerConfiguration::createServerConfiguration("localhost", -1);

        ASSERT_FALSE(config.has_value());
        EXPECT_EQ(config.error().errorCode, ConfigurationErrorCode::InvalidConfiguration);
        EXPECT_FALSE(config.error().message.empty());
    }

    TEST(ServerConfigurationTest, CreateWithPortAboveMaxReturnsError) {
        auto config = ServerConfiguration::createServerConfiguration("localhost", 65536);

        ASSERT_FALSE(config.has_value());
        EXPECT_EQ(config.error().errorCode, ConfigurationErrorCode::InvalidConfiguration);
        EXPECT_FALSE(config.error().message.empty());
    }

    TEST(ServerConfigurationTest, CreateWithNulloptHostAndPort) {
        auto config = ServerConfiguration::createServerConfiguration(std::nullopt, std::nullopt);

        ASSERT_TRUE(config.has_value());
        EXPECT_EQ(config->getHost(), "0.0.0.0");
        EXPECT_EQ(config->getPort(), 6379);
    }

    TEST(ServerConfigurationTest, CreateWithNulloptHost) {
        auto config = ServerConfiguration::createServerConfiguration(std::nullopt, 6379);

        ASSERT_TRUE(config.has_value());
        EXPECT_EQ(config->getHost(), "0.0.0.0");
        EXPECT_EQ(config->getPort(), 6379);
    }

    TEST(ServerConfigurationTest, CreateWithNulloptPort) {
        auto config = ServerConfiguration::createServerConfiguration("localhost", std::nullopt);

        ASSERT_TRUE(config.has_value());
        EXPECT_EQ(config->getHost(), "localhost");
        EXPECT_EQ(config->getPort(), 6379);
    }

    // Getter tests
    TEST(ServerConfigurationTest, GettersReturnCorrectValues) {
        auto config = ServerConfiguration::createServerConfiguration("redis.local", 7000);

        ASSERT_TRUE(config.has_value());

        const std::string &host = config->getHost();
        const int &port = config->getPort();

        EXPECT_EQ(host, "redis.local");
        EXPECT_EQ(port, 7000);
    }

    TEST(ServerConfigurationTest, GetHostReturnsConstReference) {
        auto config = ServerConfiguration::createServerConfiguration("localhost", 6379);
        ASSERT_TRUE(config.has_value());

        const std::string &host1 = config->getHost();
        const std::string &host2 = config->getHost();

        // Verify they reference the same underlying string
        EXPECT_EQ(&host1, &host2);
    }
} // namespace gmredis::config::test
