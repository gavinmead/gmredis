#include "command_selector_impl.h"

namespace gmredis::command {
    std::expected<std::shared_ptr<Command>, CommandError> DefaultCommandSelector::select(const protocol::Array& req) {
        if (req.values.size() == 0) {
            return std::unexpected(CommandError(CommandErrorCode::WrongArgumentCount, "Cannot select an empty array."));
        }

        for (const auto& val : req.values) {
            if (!std::holds_alternative<protocol::BulkString>(val)) {
                return std::unexpected(CommandError(CommandErrorCode::InvalidArgument, "All arguments must be BulkStrings"));
            }
        }

        auto commandText = std::get<protocol::BulkString>(req.values[COMMAND_INDEX]).value;

        //Convert the text to a CommandType
        auto commandType = get_command(commandText);
        if (!commandType.has_value()) {
            return std::unexpected(CommandError(CommandErrorCode::CommandNotFound, "command text is not valid"));
        }

        auto cmd = command_registry->getCommand(commandType.value());
        if (!cmd.has_value()) {
            return std::unexpected(CommandError(CommandErrorCode::CommandNotFound, "command not registered in command registry"));
        }

        return cmd.value();

    }
}