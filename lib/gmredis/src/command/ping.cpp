#include "gmredis/command/ping.h"

namespace gmredis::command {
    std::optional<CommandError> PingCommand::doValidate(const protocol::Array& arg) {
        // If the array length is greater than 2, then we know it is invalid
        if (arg.values.size() > 2) {
            return CommandError(CommandErrorCode::WrongArgumentCount, "ping takes 0 or 1 arguments");
        }

        for (const auto& val : arg.values) {
            if (!std::holds_alternative<protocol::BulkString>(val)) {
                return CommandError(CommandErrorCode::InvalidArgument, "All arguments must be BulkStrings");
            }
        }

        return std::nullopt;
    }

    std::expected<protocol::RespValue, CommandError> PingCommand::doExecute(const protocol::Array& arg) {
        if (arg.values.size() == 2) {
            return std::get<protocol::BulkString>(arg.values[1]);
        }
        return protocol::SimpleString{.value = "PONG"};
    }



}