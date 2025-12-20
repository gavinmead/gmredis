#include "gmredis/command/ping.h"

namespace gmredis::command {
    constexpr size_t MAX_PING_ARGS = 2; // command + optional message
    constexpr size_t MESSAGE_INDEX = 1;

    std::optional<CommandError> PingCommand::doValidate(const protocol::Array& arg) {
        // If the array length is greater than 2, then we know it is invalid
        if (arg.values.size() > MAX_PING_ARGS) {
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
        if (arg.values.size() == MAX_PING_ARGS) {
            const auto& bulk_str = std::get<protocol::BulkString>(arg.values[MESSAGE_INDEX]);
            return protocol::BulkString{.value = bulk_str.value, .length = bulk_str.length};
        }
        return protocol::SimpleString{.value = "PONG"};
    }

}