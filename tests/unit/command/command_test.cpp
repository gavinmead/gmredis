#include <gtest/gtest.h>
#include "gmredis/command/command.h"
#include <vector>

namespace gmredis::test {

    // ========== Parameterized Test for Valid Commands ==========

    struct ValidCommandTestCase {
        const char* input;
        command::Command expected;
        const char* description;
    };

    class ValidCommandTest : public ::testing::TestWithParam<ValidCommandTestCase> {};

    TEST_P(ValidCommandTest, ReturnsCorrectCommand) {
        const auto& params = GetParam();
        auto result = command::get_command(params.input);
        ASSERT_TRUE(result.has_value()) << "Failed for: " << params.description;
        EXPECT_EQ(result.value(), params.expected) << "Failed for: " << params.description;
    }

    struct ValidCommandTestNamer {
        std::string operator()(const ::testing::TestParamInfo<ValidCommandTestCase>& info) const {
            return info.param.description;
        }
    };

    INSTANTIATE_TEST_SUITE_P(
        CaseInsensitiveCommands,
        ValidCommandTest,
        ::testing::Values(
            // PING variations
            ValidCommandTestCase{"ping", command::Command::Ping, "ping_lowercase"},
            ValidCommandTestCase{"PING", command::Command::Ping, "PING_uppercase"},
            ValidCommandTestCase{"Ping", command::Command::Ping, "Ping_capitalized"},
            ValidCommandTestCase{"PiNg", command::Command::Ping, "PiNg_mixed_case"},
            ValidCommandTestCase{"pInG", command::Command::Ping, "pInG_alternating_case"},

            // GET variations
            ValidCommandTestCase{"get", command::Command::Get, "get_lowercase"},
            ValidCommandTestCase{"GET", command::Command::Get, "GET_uppercase"},
            ValidCommandTestCase{"Get", command::Command::Get, "Get_capitalized"},
            ValidCommandTestCase{"GeT", command::Command::Get, "GeT_mixed_case"},
            ValidCommandTestCase{"gEt", command::Command::Get, "gEt_alternating_case"},

            // SET variations
            ValidCommandTestCase{"set", command::Command::Set, "set_lowercase"},
            ValidCommandTestCase{"SET", command::Command::Set, "SET_uppercase"},
            ValidCommandTestCase{"Set", command::Command::Set, "Set_capitalized"},
            ValidCommandTestCase{"SeT", command::Command::Set, "SeT_mixed_case"}
        ),
        ValidCommandTestNamer()
    );

    // ========== Parameterized Test for Invalid Commands ==========

    class InvalidCommandTest : public ::testing::TestWithParam<std::string_view> {};

    TEST_P(InvalidCommandTest, ReturnsNullopt) {
        const auto& input = GetParam();
        auto result = command::get_command(input);
        EXPECT_FALSE(result.has_value()) << "Should return nullopt for: " << input;
    }

    INSTANTIATE_TEST_SUITE_P(
        UnknownAndInvalidCommands,
        InvalidCommandTest,
        ::testing::Values(
            // Unknown commands
            "UNKNOWN",
            "DEL",
            "HSET",
            "LPUSH",
            "PONG",

            // Empty and whitespace
            "",
            "\t",
            "\n",
            "\r",

            // Commands with spaces
            " PING",
            "PING ",
            "P I N G",

            // Partial matches
            "PI",
            "GE",
            "SE",

            // Extra characters
            "PINGS",
            "GETX",
            "SETX",

            // Invalid characters
            "12345",
            "@#$%"
        )
    );

    // ========== Parameterized Test for String Type Variations ==========

    struct StringTypeTestCase {
        std::function<std::optional<command::Command>()> test_func;
        command::Command expected;
        const char* description;
    };

    class StringTypeTest : public ::testing::TestWithParam<StringTypeTestCase> {};

    TEST_P(StringTypeTest, HandlesVariousStringTypes) {
        const auto& params = GetParam();
        auto result = params.test_func();
        ASSERT_TRUE(result.has_value()) << "Failed for: " << params.description;
        EXPECT_EQ(result.value(), params.expected) << "Failed for: " << params.description;
    }

    struct StringTypeTestNamer {
        std::string operator()(const ::testing::TestParamInfo<StringTypeTestCase>& info) const {
            return info.param.description;
        }
    };

    INSTANTIATE_TEST_SUITE_P(
        DifferentStringTypes,
        StringTypeTest,
        ::testing::Values(
            StringTypeTestCase{
                []() {
                    std::string cmd = "PING";
                    return command::get_command(cmd);
                },
                command::Command::Ping,
                "std_string"
            },
            StringTypeTestCase{
                []() {
                    std::string_view cmd = "GET";
                    return command::get_command(cmd);
                },
                command::Command::Get,
                "std_string_view"
            },
            StringTypeTestCase{
                []() {
                    const char* cmd = "SET";
                    return command::get_command(cmd);
                },
                command::Command::Set,
                "const_char_ptr"
            },
            StringTypeTestCase{
                []() {
                    std::string full = "The command is PING today";
                    std::string_view cmd = std::string_view(full).substr(15, 4);
                    return command::get_command(cmd);
                },
                command::Command::Ping,
                "substring_view"
            }
        ),
        StringTypeTestNamer()
    );

    // ========== Non-Parameterized Edge Case Tests ==========

    TEST(GetCommandTest, AllUppercaseCommandsWork) {
        EXPECT_TRUE(command::get_command("PING").has_value());
        EXPECT_TRUE(command::get_command("GET").has_value());
        EXPECT_TRUE(command::get_command("SET").has_value());
    }

    TEST(GetCommandTest, AllLowercaseCommandsWork) {
        EXPECT_TRUE(command::get_command("ping").has_value());
        EXPECT_TRUE(command::get_command("get").has_value());
        EXPECT_TRUE(command::get_command("set").has_value());
    }

    TEST(GetCommandTest, LeadingSpaceIsRejected) {
        auto result = command::get_command(" PING");
        EXPECT_FALSE(result.has_value()) << "Commands with leading space should not match";
    }

    TEST(GetCommandTest, TrailingSpaceIsRejected) {
        auto result = command::get_command("PING ");
        EXPECT_FALSE(result.has_value()) << "Commands with trailing space should not match";
    }

    TEST(GetCommandTest, InternalSpacesAreRejected) {
        auto result = command::get_command("P I N G");
        EXPECT_FALSE(result.has_value()) << "Commands with internal spaces should not match";
    }

    TEST(GetCommandTest, PartialMatchesAreRejected) {
        EXPECT_FALSE(command::get_command("PI").has_value()) << "Partial 'PI' should not match PING";
        EXPECT_FALSE(command::get_command("GE").has_value()) << "Partial 'GE' should not match GET";
    }

    TEST(GetCommandTest, ExtraCharactersAreRejected) {
        EXPECT_FALSE(command::get_command("PINGS").has_value()) << "'PINGS' should not match 'PING'";
        EXPECT_FALSE(command::get_command("GETX").has_value()) << "'GETX' should not match 'GET'";
    }

    TEST(GetCommandTest, SimilarButDifferentCommandIsRejected) {
        EXPECT_FALSE(command::get_command("PONG").has_value()) << "'PONG' should not match 'PING'";
    }

} // namespace gmredis::test