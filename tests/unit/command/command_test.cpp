#include <gtest/gtest.h>
#include "gmredis/command/command.h"

namespace gmredis::test {

    // Test fixture for command tests
    class CommandTest : public ::testing::Test {
    protected:
        void SetUp() override {
            // Setup code if needed
        }

        void TearDown() override {
            // Cleanup code if needed
        }
    };

    // ========== Valid Command Tests ==========

    TEST_F(CommandTest, GetCommand_Ping_Lowercase) {
        auto result = command::get_command("ping");
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), command::Command::Ping);
    }

    TEST_F(CommandTest, GetCommand_Ping_Uppercase) {
        auto result = command::get_command("PING");
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), command::Command::Ping);
    }

    TEST_F(CommandTest, GetCommand_Ping_MixedCase) {
        auto result = command::get_command("PiNg");
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), command::Command::Ping);
    }

    TEST_F(CommandTest, GetCommand_Ping_Capitalized) {
        auto result = command::get_command("Ping");
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), command::Command::Ping);
    }

    TEST_F(CommandTest, GetCommand_Get_Lowercase) {
        auto result = command::get_command("get");
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), command::Command::Get);
    }

    TEST_F(CommandTest, GetCommand_Get_Uppercase) {
        auto result = command::get_command("GET");
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), command::Command::Get);
    }

    TEST_F(CommandTest, GetCommand_Get_MixedCase) {
        auto result = command::get_command("GeT");
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), command::Command::Get);
    }

    TEST_F(CommandTest, GetCommand_Set_Lowercase) {
        auto result = command::get_command("set");
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), command::Command::Set);
    }

    TEST_F(CommandTest, GetCommand_Set_Uppercase) {
        auto result = command::get_command("SET");
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), command::Command::Set);
    }

    TEST_F(CommandTest, GetCommand_Set_MixedCase) {
        auto result = command::get_command("SeT");
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), command::Command::Set);
    }

    // ========== Invalid Command Tests ==========

    TEST_F(CommandTest, GetCommand_UnknownCommand) {
        auto result = command::get_command("UNKNOWN");
        EXPECT_FALSE(result.has_value());
    }

    TEST_F(CommandTest, GetCommand_EmptyString) {
        auto result = command::get_command("");
        EXPECT_FALSE(result.has_value());
    }

    TEST_F(CommandTest, GetCommand_InvalidCommand_Del) {
        auto result = command::get_command("DEL");
        EXPECT_FALSE(result.has_value());
    }

    TEST_F(CommandTest, GetCommand_InvalidCommand_Hset) {
        auto result = command::get_command("HSET");
        EXPECT_FALSE(result.has_value());
    }

    TEST_F(CommandTest, GetCommand_InvalidCommand_Lpush) {
        auto result = command::get_command("LPUSH");
        EXPECT_FALSE(result.has_value());
    }

    // ========== Edge Case Tests ==========

    TEST_F(CommandTest, GetCommand_CommandWithLeadingSpace) {
        auto result = command::get_command(" PING");
        EXPECT_FALSE(result.has_value()) << "Commands with leading space should not match";
    }

    TEST_F(CommandTest, GetCommand_CommandWithTrailingSpace) {
        auto result = command::get_command("PING ");
        EXPECT_FALSE(result.has_value()) << "Commands with trailing space should not match";
    }

    TEST_F(CommandTest, GetCommand_CommandWithSpaces) {
        auto result = command::get_command("P I N G");
        EXPECT_FALSE(result.has_value()) << "Commands with internal spaces should not match";
    }

    TEST_F(CommandTest, GetCommand_PartialMatch_Pi) {
        auto result = command::get_command("PI");
        EXPECT_FALSE(result.has_value()) << "Partial command names should not match";
    }

    TEST_F(CommandTest, GetCommand_PartialMatch_Ge) {
        auto result = command::get_command("GE");
        EXPECT_FALSE(result.has_value()) << "Partial command names should not match";
    }

    TEST_F(CommandTest, GetCommand_ExtraCharacters_Pings) {
        auto result = command::get_command("PINGS");
        EXPECT_FALSE(result.has_value()) << "Commands with extra characters should not match";
    }

    TEST_F(CommandTest, GetCommand_ExtraCharacters_Getx) {
        auto result = command::get_command("GETX");
        EXPECT_FALSE(result.has_value()) << "Commands with extra characters should not match";
    }

    TEST_F(CommandTest, GetCommand_SimilarCommand_Pong) {
        auto result = command::get_command("PONG");
        EXPECT_FALSE(result.has_value()) << "Similar but different command should not match";
    }

    TEST_F(CommandTest, GetCommand_NumbersOnly) {
        auto result = command::get_command("12345");
        EXPECT_FALSE(result.has_value());
    }

    TEST_F(CommandTest, GetCommand_SpecialCharacters) {
        auto result = command::get_command("@#$%");
        EXPECT_FALSE(result.has_value());
    }

    TEST_F(CommandTest, GetCommand_Tab) {
        auto result = command::get_command("\t");
        EXPECT_FALSE(result.has_value());
    }

    TEST_F(CommandTest, GetCommand_Newline) {
        auto result = command::get_command("\n");
        EXPECT_FALSE(result.has_value());
    }

    TEST_F(CommandTest, GetCommand_CarriageReturn) {
        auto result = command::get_command("\r");
        EXPECT_FALSE(result.has_value());
    }

    // ========== Case Sensitivity Boundary Tests ==========

    TEST_F(CommandTest, GetCommand_AllUppercase_AllCommands) {
        EXPECT_TRUE(command::get_command("PING").has_value());
        EXPECT_TRUE(command::get_command("GET").has_value());
        EXPECT_TRUE(command::get_command("SET").has_value());
    }

    TEST_F(CommandTest, GetCommand_AllLowercase_AllCommands) {
        EXPECT_TRUE(command::get_command("ping").has_value());
        EXPECT_TRUE(command::get_command("get").has_value());
        EXPECT_TRUE(command::get_command("set").has_value());
    }

    TEST_F(CommandTest, GetCommand_AlternatingCase_Ping) {
        auto result = command::get_command("pInG");
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), command::Command::Ping);
    }

    TEST_F(CommandTest, GetCommand_AlternatingCase_Get) {
        auto result = command::get_command("gEt");
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), command::Command::Get);
    }

    // ========== String View Tests ==========

    TEST_F(CommandTest, GetCommand_FromStdString) {
        std::string cmd = "PING";
        auto result = command::get_command(cmd);
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), command::Command::Ping);
    }

    TEST_F(CommandTest, GetCommand_FromStringView) {
        std::string_view cmd = "GET";
        auto result = command::get_command(cmd);
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), command::Command::Get);
    }

    TEST_F(CommandTest, GetCommand_FromCharArray) {
        const char* cmd = "SET";
        auto result = command::get_command(cmd);
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), command::Command::Set);
    }

    TEST_F(CommandTest, GetCommand_FromSubstring) {
        std::string full = "The command is PING today";
        std::string_view cmd = std::string_view(full).substr(15, 4); // "PING"
        auto result = command::get_command(cmd);
        ASSERT_TRUE(result.has_value());
        EXPECT_EQ(result.value(), command::Command::Ping);
    }

    // ========== Comprehensive Coverage Test ==========

    TEST_F(CommandTest, GetCommand_AllValidCommandsReturnCorrectEnum) {
        // Ensure all commands in the enum are properly mapped
        std::vector<std::pair<std::string_view, command::Command>> test_cases = {
            {"ping", command::Command::Ping},
            {"PING", command::Command::Ping},
            {"get", command::Command::Get},
            {"GET", command::Command::Get},
            {"set", command::Command::Set},
            {"SET", command::Command::Set},
        };

        for (const auto& [input, expected] : test_cases) {
            auto result = command::get_command(input);
            ASSERT_TRUE(result.has_value()) << "Failed for input: " << input;
            EXPECT_EQ(result.value(), expected) << "Failed for input: " << input;
        }
    }

} // namespace gmredis::test