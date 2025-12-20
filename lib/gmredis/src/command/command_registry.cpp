#include "command_registry_impl.h"

namespace gmredis::command {

    std::optional<CommandRegistryError> DefaultCommandRegistry::registerCommand(CommandType commandType, std::shared_ptr<Command> command) noexcept {
        if (command == nullptr) {
            return CommandRegistryError::NullCommand;
        }

        if (commands.contains(commandType)) {
            return CommandRegistryError::AlreadyRegistered;
        }

        commands.insert({commandType, command});

        return std::nullopt;
    }

    std::expected<std::shared_ptr<Command>, CommandRegistryError> DefaultCommandRegistry::getCommand(CommandType commandType) const noexcept {
        if (!commands.contains(commandType)) {
            return std::unexpected(CommandRegistryError::CommandNotFound);
        }
        return commands.at(commandType);
    }

}