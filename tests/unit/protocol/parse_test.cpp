#include <gtest/gtest.h>
#include "gmredis/protocol/parse.h"

namespace gmredis::test {
    TEST(ParseTest, SimpleStringParse) {
        std::string_view input = "+Hello, World!\r\n";
        auto result = protocol::parse_simple_string(input);
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(std::get<protocol::SimpleString>(result.value()).value, "Hello, World!");
        EXPECT_EQ(input, ""); // input should be fully consumed
    }
    TEST(ParseTest, SimpleStringParseEmpty) {
        std::string_view input = "+\r\n";
        auto result = protocol::parse_simple_string(input);
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(std::get<protocol::SimpleString>(result.value()).value, "");
        EXPECT_EQ(input, ""); // input should be fully consumed
    }
    TEST(ParseTest, SimpleStringParseEmptyStringView) {
        std::string_view input = "";
        auto result = protocol::parse_simple_string(input);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), protocol::ParseError::Incomplete);
        EXPECT_EQ(input, ""); // input should remain unchanged
    }
    TEST(ParseTest, SimpleStringParseInvalidPrefix) {
        std::string_view input = "-Invalid Prefix\r\n";
        auto result = protocol::parse_simple_string(input);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), protocol::ParseError::Invalid);
        EXPECT_EQ(input, "-Invalid Prefix\r\n"); // input should remain unchanged
    }

    TEST(ParseTest, SimpleErrorParse) {
        std::string_view input = "-ERR unknown command\r\n";
        auto result = protocol::parse_error(input);
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(std::get<protocol::SimpleError>(result.value()).value, "ERR unknown command");
        EXPECT_EQ(input, "");
    }

    TEST(ParseTest, SimpleErrorParseEmpty) {
        std::string_view input = "-\r\n";
        auto result = protocol::parse_error(input);
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(std::get<protocol::SimpleError>(result.value()).value, "");
        EXPECT_EQ(input, "");
    }

    TEST(ParseTest, SimpleErrorParseEmptyStringView) {
        std::string_view input = "";
        auto result = protocol::parse_error(input);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), protocol::ParseError::Incomplete);
        EXPECT_EQ(input, "");
    }

    TEST(ParseTest, SimpleErrorParseInvalidPrefix) {
        std::string_view input = "+Not an error\r\n";
        auto result = protocol::parse_error(input);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), protocol::ParseError::Invalid);
        EXPECT_EQ(input, "+Not an error\r\n");
    }

    TEST(ParseTest, SimpleErrorParseIncomplete) {
        std::string_view input = "-ERR partial";
        auto result = protocol::parse_error(input);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), protocol::ParseError::Incomplete);
        EXPECT_EQ(input, "-ERR partial");
    }

    TEST(ParseTest, SimpleErrorParseWithRemainingData) {
        std::string_view input = "-ERR first\r\n+OK\r\n";
        auto result = protocol::parse_error(input);
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(std::get<protocol::SimpleError>(result.value()).value, "ERR first");
        EXPECT_EQ(input, "+OK\r\n");
    }

}