#include <gtest/gtest.h>
#include "gmredis/protocol/parse.h"
#include <vector>

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

    TEST(ParseTest, ParseFunctionOk) {
        std::string_view input = "+Hello, Parse!\r\n";
        auto result = protocol::parse(input);
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(std::get<protocol::SimpleString>(result.value()).value, "Hello, Parse!");
        EXPECT_EQ(input, "");
    }

    TEST(ParseTest, ParseFunctionUnsupported) {
        std::string_view input = "}1000\r\n"; // } is not supported
        auto result = protocol::parse(input);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), protocol::ParseError::Unsupported);
        EXPECT_EQ(input, "}1000\r\n");
    }

    TEST(ParseTest, ParseFunctionEmpty) {
        std::string_view input = "";
        auto result = protocol::parse(input);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), protocol::ParseError::Incomplete);
        EXPECT_EQ(input, "");
    }

    TEST(ParseTest, BulkStringOk) {
        std::string_view input = "$5\r\nhello\r\n";
        auto result = protocol::parse(input);
        EXPECT_TRUE(result.has_value());

        auto bulk_string = std::get<protocol::BulkString>(result.value());
        EXPECT_EQ(bulk_string.value, "hello");
        EXPECT_EQ(bulk_string.length, 5);
    }

    TEST(ParseTest, BulkStringEmpty) {
        std::string_view input = "$0\r\n\r\n";

        auto result = protocol::parse(input);
        EXPECT_TRUE(result.has_value());

        auto bulk_string = std::get<protocol::BulkString>(result.value());
        EXPECT_EQ(bulk_string.value, "");
        EXPECT_EQ(bulk_string.length, 0);
    }

    TEST(ParseTest, BulkStringEmptyStringInput) {
        std::string_view input = "";
        auto result = protocol::parse(input);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), protocol::ParseError::Incomplete);
    }

    TEST(ParseTest, BulkStringInvalid) {
        std::string_view input = "}0\r\n\r\n";
        auto result = protocol::parse_bulk_string(input);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), protocol::ParseError::Invalid);
        EXPECT_EQ(input, "}0\r\n\r\n");
    }

    TEST(ParseTest, BulkStringEscapedString) {
        std::string_view input = "$7\r\nhel\r\nlo\r\n";
        auto result = protocol::parse_bulk_string(input);
        EXPECT_TRUE(result.has_value());
        auto bulk_string = std::get<protocol::BulkString>(result.value());
        EXPECT_EQ(bulk_string.value, "hel\r\nlo");
    }

    TEST(ParseTest, BulkStringEscapedIncomplete) {
        std::string_view input = "$7\r\nhel\r\nlo";
        auto result = protocol::parse_bulk_string(input);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), protocol::ParseError::Incomplete);
        EXPECT_EQ(input, "$7\r\nhel\r\nlo");
    }

    TEST(ParseTest, PositiveIntegerOK) {
        std::string_view input = ":123\r\n";
        auto result = protocol::parse_integer(input);
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(std::get<protocol::Integer>(result.value()).value, 123);
        EXPECT_EQ(input, "");
    }

    TEST(ParseTest, ExplicitPositiveIntegerOK) {
        std::string_view input = ":+123\r\n";
        auto result = protocol::parse_integer(input);
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(std::get<protocol::Integer>(result.value()).value, 123);
        EXPECT_EQ(input, "");
    }

    TEST(ParseTest, NegativeIntegerOK) {
        std::string_view input = ":-123\r\n";
        auto result = protocol::parse_integer(input);
        EXPECT_TRUE(result.has_value());
        EXPECT_EQ(std::get<protocol::Integer>(result.value()).value, -123);
        EXPECT_EQ(input, "");
    }

    TEST(ParseTest, IncompleteInteger) {
        std::string_view input = ":-123";
        auto result = protocol::parse_integer(input);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), protocol::ParseError::Incomplete);
        EXPECT_EQ(input, ":-123");
    }

    TEST(ParseTest, InvalidPrefixInteger) {
        std::string_view input = "}-123";
        auto result = protocol::parse_integer(input);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), protocol::ParseError::Invalid);
        EXPECT_EQ(input, "}-123");
    }

    TEST(ParseTest, MixedIntgerText) {
        std::string_view input = ":123abc\r\n";
        auto result = protocol::parse_integer(input);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), protocol::ParseError::Invalid);
    }

    TEST(ParseTest, InvalidInteger) {
        std::string_view input = ":abc\r\n";
        auto result = protocol::parse_integer(input);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), protocol::ParseError::Invalid);
    }

    TEST(ParseTest, ArraySingleType) {
        std::string_view input = "*2\r\n$5\r\nhello\r\n$5\r\nworld\r\n";
        auto result = protocol::parse_array(input);
        EXPECT_TRUE(result.has_value());

        std::vector<protocol::RespValue> const expected_results = {protocol::BulkString{.value = "hello", .length = 5},
                                                    protocol::BulkString{.value = "world", .length = 5}};

        auto array_results = std::get<protocol::Array>(result.value());

        EXPECT_EQ(array_results.values, expected_results);

        EXPECT_EQ(input, "");
    }

    TEST(ParseTest, ArrayMultiType) {
        std::string_view input = "*4\r\n$5\r\nhello\r\n:123\r\n$5\r\nworld\r\n:-321\r\n";

        auto result = protocol::parse_array(input);
        EXPECT_TRUE(result.has_value());

        std::vector<protocol::RespValue> const expected_results = {protocol::BulkString{.value = "hello", .length = 5},
                                                    protocol::Integer{.value = 123},
                                                    protocol::BulkString{.value = "world", .length = 5},
                                                    protocol::Integer{.value = -321}};

        auto array_results = std::get<protocol::Array>(result.value());

        EXPECT_EQ(array_results.values, expected_results);

        EXPECT_EQ(input, "");
    }

    TEST(ParseTest, ArrayEmptyString) {
        std::string_view input = "";
        auto result = protocol::parse_array(input);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), protocol::ParseError::Incomplete);
    }

    TEST(ParseTest, ArrayInvalid) {
        std::string_view input = "}4\r\n$5\r\nhello\r\n$5\r\nworld\r\n";
        auto result = protocol::parse_array(input);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), protocol::ParseError::Invalid);
        EXPECT_EQ(input, "}4\r\n$5\r\nhello\r\n$5\r\nworld\r\n");
    }

    TEST(ParseTest, ArrayDeclaredLengthGreaterThanElements) {
        // Declares 3 elements but only provides 2
        std::string_view input = "*3\r\n$5\r\nhello\r\n$5\r\nworld\r\n";
        auto result = protocol::parse_array(input);
        EXPECT_FALSE(result.has_value());
        EXPECT_EQ(result.error(), protocol::ParseError::Incomplete);
    }

    TEST(ParseTest, ArrayDeclaredLengthLessThanElements) {
        // Declares 2 elements but input has more data after
        std::string_view input = "*2\r\n$5\r\nhello\r\n$5\r\nworld\r\n$5\r\nextra\r\n";
        auto result = protocol::parse_array(input);
        EXPECT_TRUE(result.has_value());

        auto array_results = std::get<protocol::Array>(result.value());
        EXPECT_EQ(array_results.values.size(), 2);

        // Extra data remains unconsumed
        EXPECT_EQ(input, "$5\r\nextra\r\n");
    }

    TEST(ParseTest, ArrayEmptyArray) {
        std::string_view input = "*0\r\n";
        auto result = protocol::parse_array(input);
        EXPECT_TRUE(result.has_value());

        auto array_results = std::get<protocol::Array>(result.value());
        EXPECT_EQ(array_results.values.size(), 0);
        EXPECT_EQ(input, "");
    }

}