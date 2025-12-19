#include <gtest/gtest.h>
#include "gmredis/command/base_command.h"

namespace gmredis::test {

    class FakeCommand : public command::BaseCommand {
    public:
        FakeCommand() : FakeCommand(false, false) {
        };

        FakeCommand(const bool fail_validation, const bool fail_execution) {
            fail_execute = fail_execution;
            fail_validate = fail_validation;
            pre_execute_called = false;
            post_execute_called = false;
            pre_validate_called = false;
            post_validate_called = false;
            do_execute_called = false;
            do_validate_called = false;
        }

        bool failValidate() const { return fail_validate; }
        bool failExecute() const { return fail_execute; }
        bool preValidateCalled() const { return pre_validate_called; }
        bool doValidateCalled() const { return do_validate_called; }
        bool postValidateCalled() const { return post_validate_called; }
        bool preExecuteCalled() const { return pre_execute_called; }
        bool doExecuteCalled() const { return do_execute_called; }
        bool postExecuteCalled() const { return post_execute_called; }

    protected:
        void preExecute([[maybe_unused]] const protocol::Array& arg) override {
            pre_execute_called = true;
        }

        void postExecute([[maybe_unused]] const protocol::Array &arg,
                         [[maybe_unused]] std::expected<protocol::RespValue, command::CommandError> &result) override {
            post_execute_called = true;
        }

        std::expected<protocol::RespValue, command::CommandError> doExecute([[maybe_unused]] const protocol::Array &arg) override {
            do_execute_called = true;
            if (fail_execute) {
                return std::unexpected<command::CommandError>(command::CommandError(command::CommandErrorCode::ExecutionFailed, "error"));
            }
            return protocol::SimpleString("ok");
        }

        void preValidate([[maybe_unused]] const protocol::Array& arg) override {
            pre_validate_called = true;
        }

        void postValidate([[maybe_unused]] const protocol::Array &arg) override {
            post_validate_called = true;
        }

        std::optional<command::CommandError> doValidate([[maybe_unused]] const protocol::Array &arg) override {
            do_validate_called = true;
            if (fail_validate) {
                return command::CommandError(command::CommandErrorCode::InvalidArgument, "error");
            }
            return std::nullopt;
        }

    private:
        bool fail_validate;
        bool fail_execute;
        bool pre_validate_called;
        bool do_validate_called;
        bool post_validate_called;
        bool pre_execute_called;
        bool do_execute_called;
        bool post_execute_called;
    };

    TEST(BaseCommandTest, HappyPathValidation) {
        auto command = FakeCommand();
        auto arg = protocol::Array{.values={}};

        auto result = command.validate(arg);
        ASSERT_FALSE(result.has_value());

        ASSERT_TRUE(command.doValidateCalled());
        ASSERT_TRUE(command.preValidateCalled());
        ASSERT_TRUE(command.postValidateCalled());
    }

    TEST(BaseCommandTest, HappyPathExecution) {
        auto command = FakeCommand();
        auto arg = protocol::Array{.values={}};
        auto result = command.execute(arg);

        ASSERT_TRUE(result.has_value());
        ASSERT_TRUE(command.doExecuteCalled());
        ASSERT_TRUE(command.preExecuteCalled());
        ASSERT_TRUE(command.postExecuteCalled());
    }

    TEST(BaseCommandTest, ValidationFailed) {
        auto command = FakeCommand(true, false);
        auto arg = protocol::Array{.values={}};
        auto result = command.validate(arg);

        ASSERT_TRUE(result.has_value());
        ASSERT_TRUE(command.doValidateCalled());
        ASSERT_TRUE(command.preValidateCalled());
        ASSERT_TRUE(command.postValidateCalled());
    }

    TEST(BaseCommandTest, ExecutionFailed) {
        auto command = FakeCommand(false, true);
        auto arg = protocol::Array{.values={}};
        auto result = command.execute(arg);

        ASSERT_FALSE(result.has_value());
        ASSERT_TRUE(command.doExecuteCalled());
        ASSERT_TRUE(command.preExecuteCalled());
        ASSERT_TRUE(command.postExecuteCalled());
    }

}