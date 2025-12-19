#include "gmredis/command/base_command.h"

namespace gmredis::command {

    std::optional<CommandError> BaseCommand::validate(const protocol::Array& arg) {
        preValidate(arg);

        auto validationResult = doValidate(arg);

        postValidate(arg);

        return validationResult;
    }

    std::expected<protocol::RespValue, CommandError> BaseCommand::execute(const protocol::Array& arg) {
        preExecute(arg);

        auto executionResult = doExecute(arg);

        postExecute(arg, executionResult);

        return executionResult;
    }

}