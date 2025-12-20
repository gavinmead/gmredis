#include <gtest/gtest.h>
#include "gmredis/command/ping.h"

namespace gmredis::test {

    TEST(PingCommandTest, ValidateNoArguments) {
        auto cmd = command::PingCommand();
        auto arg = protocol::Array{.values = {
            protocol::BulkString{.value = "PING", .length = 4}
        }};

        auto result = cmd.validate(arg);
        ASSERT_FALSE(result.has_value());
    }

    TEST(PingCommandTest, ValidateWithOneArgument) {
        auto cmd = command::PingCommand();
        auto arg = protocol::Array{.values = {
            protocol::BulkString{.value = "PING", .length = 4},
            protocol::BulkString{.value = "hello", .length = 5}
        }};

        auto result = cmd.validate(arg);
        ASSERT_FALSE(result.has_value());
    }

    TEST(PingCommandTest, ValidateTooManyArguments) {
        auto cmd = command::PingCommand();
        auto arg = protocol::Array{.values = {
            protocol::BulkString{.value = "PING", .length = 4},
            protocol::BulkString{.value = "arg1", .length = 4},
            protocol::BulkString{.value = "arg2", .length = 4}
        }};

        auto result = cmd.validate(arg);
        ASSERT_TRUE(result.has_value());
        ASSERT_EQ(result->code, command::CommandErrorCode::WrongArgumentCount);
        ASSERT_EQ(result->message, "ping takes 0 or 1 arguments");
    }

    TEST(PingCommandTest, ValidateInvalidArgumentType) {
        auto cmd = command::PingCommand();
        auto arg = protocol::Array{.values = {
            protocol::BulkString{.value = "PING", .length = 4},
            protocol::SimpleString{.value = "notbulk"}  // Invalid type
        }};

        auto result = cmd.validate(arg);
        ASSERT_TRUE(result.has_value());
        ASSERT_EQ(result->code, command::CommandErrorCode::InvalidArgument);
        ASSERT_EQ(result->message, "All arguments must be BulkStrings");
    }

    TEST(PingCommandTest, ExecuteNoArguments) {
        auto cmd = command::PingCommand();
        auto arg = protocol::Array{.values = {
            protocol::BulkString{.value = "PING", .length = 4}
        }};

        auto result = cmd.execute(arg);
        ASSERT_TRUE(result.has_value());

        auto* simpleStr = std::get_if<protocol::SimpleString>(&result.value());
        ASSERT_NE(simpleStr, nullptr);
        ASSERT_EQ(simpleStr->value, "PONG");
    }

    TEST(PingCommandTest, ExecuteWithArgument) {
        auto cmd = command::PingCommand();
        auto arg = protocol::Array{.values = {
            protocol::BulkString{.value = "PING", .length = 4},
            protocol::BulkString{.value = "hello world", .length = 11}
        }};

        auto result = cmd.execute(arg);
        ASSERT_TRUE(result.has_value());

        auto* bulkStr = std::get_if<protocol::BulkString>(&result.value());
        ASSERT_NE(bulkStr, nullptr);
        ASSERT_EQ(bulkStr->value, "hello world");
        ASSERT_EQ(bulkStr->length, 11);
    }

    TEST(PingCommandTest, ExecuteEchosExactArgument) {
        auto cmd = command::PingCommand();
        auto arg = protocol::Array{.values = {
            protocol::BulkString{.value = "PING", .length = 4},
            protocol::BulkString{.value = "test123", .length = 7}
        }};

        auto result = cmd.execute(arg);
        ASSERT_TRUE(result.has_value());

        auto* bulkStr = std::get_if<protocol::BulkString>(&result.value());
        ASSERT_NE(bulkStr, nullptr);
        ASSERT_EQ(bulkStr->value, "test123");
    }

}