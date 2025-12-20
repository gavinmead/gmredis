#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "gmredis/command/command.h"
#include "command/command_selector_impl.h"

using ::testing::Return;
using ::testing::_;

namespace gmredis::test {

    class MockCommand : public command::Command {
    public:
        MOCK_METHOD(std::optional<command::CommandError>, validate, (const protocol::Array&), (override));
        MOCK_METHOD((std::expected<protocol::RespValue, command::CommandError>), execute, (const protocol::Array&), (override));
    };

    class MockCommandRegistry : public command::CommandRegistry {
    public:
        MOCK_METHOD(std::optional<command::CommandRegistryError>, registerCommand,
                   (command::CommandType, std::shared_ptr<command::Command>), (noexcept, override));
        MOCK_METHOD((std::expected<std::shared_ptr<command::Command>, command::CommandRegistryError>),
                   getCommand, (command::CommandType), (const, noexcept, override));
    };

    TEST(CommandSelectorTest, SelectValidPingCommand) {
        auto mockRegistry = std::make_unique<MockCommandRegistry>();
        auto mockCmd = std::make_shared<MockCommand>();

        EXPECT_CALL(*mockRegistry, getCommand(command::CommandType::Ping))
            .WillOnce(Return(mockCmd));

        auto selector = command::DefaultCommandSelector(std::move(mockRegistry));

        protocol::Array req;
        req.values.push_back(protocol::BulkString("PING"));

        auto result = selector.select(req);
        ASSERT_TRUE(result.has_value());
        ASSERT_EQ(result.value(), mockCmd);
    }

    TEST(CommandSelectorTest, SelectValidPingCommandLowercase) {
        auto mockRegistry = std::make_unique<MockCommandRegistry>();
        auto mockCmd = std::make_shared<MockCommand>();

        EXPECT_CALL(*mockRegistry, getCommand(command::CommandType::Ping))
            .WillOnce(Return(mockCmd));

        auto selector = command::DefaultCommandSelector(std::move(mockRegistry));

        protocol::Array req;
        req.values.push_back(protocol::BulkString("ping"));

        auto result = selector.select(req);
        ASSERT_TRUE(result.has_value());
        ASSERT_EQ(result.value(), mockCmd);
    }

    TEST(CommandSelectorTest, SelectValidGetCommand) {
        auto mockRegistry = std::make_unique<MockCommandRegistry>();
        auto mockCmd = std::make_shared<MockCommand>();

        EXPECT_CALL(*mockRegistry, getCommand(command::CommandType::Get))
            .WillOnce(Return(mockCmd));

        auto selector = command::DefaultCommandSelector(std::move(mockRegistry));

        protocol::Array req;
        req.values.push_back(protocol::BulkString("GET"));
        req.values.push_back(protocol::BulkString("key1"));

        auto result = selector.select(req);
        ASSERT_TRUE(result.has_value());
        ASSERT_EQ(result.value(), mockCmd);
    }

    TEST(CommandSelectorTest, EmptyArrayError) {
        auto mockRegistry = std::make_unique<MockCommandRegistry>();
        auto selector = command::DefaultCommandSelector(std::move(mockRegistry));

        protocol::Array req;  // Empty array

        auto result = selector.select(req);
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error().code, command::CommandErrorCode::WrongArgumentCount);
    }

    TEST(CommandSelectorTest, NonBulkStringArgumentError) {
        auto mockRegistry = std::make_unique<MockCommandRegistry>();
        auto selector = command::DefaultCommandSelector(std::move(mockRegistry));

        protocol::Array req;
        req.values.push_back(protocol::SimpleString("PING"));  // Should be BulkString

        auto result = selector.select(req);
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error().code, command::CommandErrorCode::InvalidArgument);
    }

    TEST(CommandSelectorTest, MixedArgumentTypesError) {
        auto mockRegistry = std::make_unique<MockCommandRegistry>();
        auto selector = command::DefaultCommandSelector(std::move(mockRegistry));

        protocol::Array req;
        req.values.push_back(protocol::BulkString("SET"));
        req.values.push_back(protocol::Integer(123));  // Should be BulkString

        auto result = selector.select(req);
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error().code, command::CommandErrorCode::InvalidArgument);
    }

    TEST(CommandSelectorTest, UnknownCommandError) {
        auto mockRegistry = std::make_unique<MockCommandRegistry>();
        auto selector = command::DefaultCommandSelector(std::move(mockRegistry));

        protocol::Array req;
        req.values.push_back(protocol::BulkString("UNKNOWN"));

        auto result = selector.select(req);
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error().code, command::CommandErrorCode::CommandNotFound);
    }

    TEST(CommandSelectorTest, CommandNotInRegistryError) {
        auto mockRegistry = std::make_unique<MockCommandRegistry>();

        EXPECT_CALL(*mockRegistry, getCommand(command::CommandType::Ping))
            .WillOnce(Return(std::unexpected(command::CommandRegistryError::CommandNotFound)));

        auto selector = command::DefaultCommandSelector(std::move(mockRegistry));

        protocol::Array req;
        req.values.push_back(protocol::BulkString("PING"));

        auto result = selector.select(req);
        ASSERT_FALSE(result.has_value());
        ASSERT_EQ(result.error().code, command::CommandErrorCode::CommandNotFound);
    }

    TEST(CommandSelectorTest, CaseInsensitiveCommandSelection) {
        auto mockRegistry = std::make_unique<MockCommandRegistry>();
        auto mockCmd = std::make_shared<MockCommand>();

        // Expect multiple calls for different case variations
        EXPECT_CALL(*mockRegistry, getCommand(command::CommandType::Ping))
            .Times(5)
            .WillRepeatedly(Return(mockCmd));

        auto selector = command::DefaultCommandSelector(std::move(mockRegistry));

        // Test various case combinations
        std::vector<std::string> variations = {"PING", "ping", "Ping", "PiNg", "pInG"};

        for (const auto& variation : variations) {
            protocol::Array req;
            req.values.push_back(protocol::BulkString(variation));
            auto result = selector.select(req);
            ASSERT_TRUE(result.has_value()) << "Failed for variation: " << variation;
            ASSERT_EQ(result.value(), mockCmd);
        }
    }

    TEST(CommandSelectorTest, CommandWithMultipleArguments) {
        auto mockRegistry = std::make_unique<MockCommandRegistry>();
        auto mockCmd = std::make_shared<MockCommand>();

        EXPECT_CALL(*mockRegistry, getCommand(command::CommandType::Set))
            .WillOnce(Return(mockCmd));

        auto selector = command::DefaultCommandSelector(std::move(mockRegistry));

        protocol::Array req;
        req.values.push_back(protocol::BulkString("SET"));
        req.values.push_back(protocol::BulkString("mykey"));
        req.values.push_back(protocol::BulkString("myvalue"));
        req.values.push_back(protocol::BulkString("EX"));
        req.values.push_back(protocol::BulkString("3600"));

        auto result = selector.select(req);
        ASSERT_TRUE(result.has_value());
        ASSERT_EQ(result.value(), mockCmd);
    }

    TEST(CommandSelectorTest, SelectDifferentCommands) {
        auto mockRegistry = std::make_unique<MockCommandRegistry>();
        auto pingCmd = std::make_shared<MockCommand>();
        auto getCmd = std::make_shared<MockCommand>();
        auto setCmd = std::make_shared<MockCommand>();

        EXPECT_CALL(*mockRegistry, getCommand(command::CommandType::Ping))
            .WillOnce(Return(pingCmd));
        EXPECT_CALL(*mockRegistry, getCommand(command::CommandType::Get))
            .WillOnce(Return(getCmd));
        EXPECT_CALL(*mockRegistry, getCommand(command::CommandType::Set))
            .WillOnce(Return(setCmd));

        auto selector = command::DefaultCommandSelector(std::move(mockRegistry));

        // Test PING
        protocol::Array pingReq;
        pingReq.values.push_back(protocol::BulkString("PING"));
        auto pingResult = selector.select(pingReq);
        ASSERT_TRUE(pingResult.has_value());
        ASSERT_EQ(pingResult.value(), pingCmd);

        // Test GET
        protocol::Array getReq;
        getReq.values.push_back(protocol::BulkString("GET"));
        getReq.values.push_back(protocol::BulkString("key"));
        auto getResult = selector.select(getReq);
        ASSERT_TRUE(getResult.has_value());
        ASSERT_EQ(getResult.value(), getCmd);

        // Test SET
        protocol::Array setReq;
        setReq.values.push_back(protocol::BulkString("SET"));
        setReq.values.push_back(protocol::BulkString("key"));
        setReq.values.push_back(protocol::BulkString("value"));
        auto setResult = selector.select(setReq);
        ASSERT_TRUE(setResult.has_value());
        ASSERT_EQ(setResult.value(), setCmd);
    }

    TEST(CommandSelectorTest, AllArgumentsMustBeBulkStrings) {
        auto mockRegistry = std::make_unique<MockCommandRegistry>();
        auto selector = command::DefaultCommandSelector(std::move(mockRegistry));

        // Test with Integer in arguments
        protocol::Array req1;
        req1.values.push_back(protocol::BulkString("SET"));
        req1.values.push_back(protocol::BulkString("key"));
        req1.values.push_back(protocol::Integer(42));
        auto result1 = selector.select(req1);
        ASSERT_FALSE(result1.has_value());
        ASSERT_EQ(result1.error().code, command::CommandErrorCode::InvalidArgument);

        // Test with SimpleString in arguments
        protocol::Array req2;
        req2.values.push_back(protocol::BulkString("GET"));
        req2.values.push_back(protocol::SimpleString("key"));
        auto result2 = selector.select(req2);
        ASSERT_FALSE(result2.has_value());
        ASSERT_EQ(result2.error().code, command::CommandErrorCode::InvalidArgument);

        // Test with Array in arguments
        protocol::Array req3;
        req3.values.push_back(protocol::BulkString("SET"));
        protocol::Array nestedArray;
        req3.values.push_back(nestedArray);
        auto result3 = selector.select(req3);
        ASSERT_FALSE(result3.has_value());
        ASSERT_EQ(result3.error().code, command::CommandErrorCode::InvalidArgument);
    }

    TEST(CommandSelectorTest, SetCommandWithValidArguments) {
        auto mockRegistry = std::make_unique<MockCommandRegistry>();
        auto mockCmd = std::make_shared<MockCommand>();

        EXPECT_CALL(*mockRegistry, getCommand(command::CommandType::Set))
            .WillOnce(Return(mockCmd));

        auto selector = command::DefaultCommandSelector(std::move(mockRegistry));

        protocol::Array req;
        req.values.push_back(protocol::BulkString("SET"));
        req.values.push_back(protocol::BulkString("key"));
        req.values.push_back(protocol::BulkString("value"));

        auto result = selector.select(req);
        ASSERT_TRUE(result.has_value());
        ASSERT_EQ(result.value(), mockCmd);
    }
}