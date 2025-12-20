#include <gtest/gtest.h>
#include "gmredis/command/command.h"
#include "command/command_registry_impl.h"

namespace gmredis::test {
    class TestCommand : public command::Command {
    public:
        std::optional<command::CommandError> validate(const protocol::Array&) override {
            return std::nullopt;  // Always valid
        }

        std::expected<protocol::RespValue, command::CommandError> execute(const protocol::Array&) override {
            return protocol::SimpleString("OK");  // Simple stub response
        }
    };

    TEST(CommandRegistryTest, RegisterAndGetOK) {
        auto registry = command::DefaultCommandRegistry();
        auto cmd = std::make_shared<TestCommand>();

        auto result = registry.registerCommand(command::CommandType::Ping, cmd);
        ASSERT_FALSE(result.has_value());

        auto returnedCmd = registry.getCommand(command::CommandType::Ping);
        ASSERT_TRUE(returnedCmd.has_value());
        ASSERT_EQ(returnedCmd.value(), cmd);
    }

    TEST(CommandRegistryTest, NullCommandRegistration) {
        auto registry = command::DefaultCommandRegistry();
        auto result = registry.registerCommand(command::CommandType::Ping, nullptr);

        ASSERT_TRUE(result.has_value());
        ASSERT_EQ(result.value(), command::CommandRegistryError::NullCommand);
    }

    TEST(CommandRegistryTest, AlreadyRegistered) {
        auto registry = command::DefaultCommandRegistry();
        auto cmd = std::make_shared<TestCommand>();
        auto result = registry.registerCommand(command::CommandType::Ping, cmd);

        ASSERT_FALSE(result.has_value());

        auto duplicateCmd = std::make_shared<TestCommand>();
        auto duplicateResult = registry.registerCommand(command::CommandType::Ping, duplicateCmd);
        ASSERT_TRUE(duplicateResult.has_value());
        ASSERT_EQ(duplicateResult.value(), command::CommandRegistryError::AlreadyRegistered);
    }

    TEST(CommandRegistryTest, CommandNotFound) {
        auto registry = command::DefaultCommandRegistry();
        auto result = registry.getCommand(command::CommandType::Ping);
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error(), command::CommandRegistryError::CommandNotFound);
    }

    TEST(CommandRegistryTest, MultiRegistrationAndGet) {
        auto registry = command::DefaultCommandRegistry();
        auto cmd1 = std::make_shared<TestCommand>();
        auto cmd2 = std::make_shared<TestCommand>();

        auto result = registry.registerCommand(command::CommandType::Ping, cmd1);
        ASSERT_FALSE(result.has_value());

        result = registry.registerCommand(command::CommandType::Get, cmd2);
        ASSERT_FALSE(result.has_value());

        auto returnedCmd1 = registry.getCommand(command::CommandType::Ping);
        auto returnedCmd2 = registry.getCommand(command::CommandType::Get);

        ASSERT_EQ(returnedCmd1.value(), cmd1);
        ASSERT_EQ(returnedCmd2.value(), cmd2);
    }

}