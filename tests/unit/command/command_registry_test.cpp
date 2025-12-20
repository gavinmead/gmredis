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
}