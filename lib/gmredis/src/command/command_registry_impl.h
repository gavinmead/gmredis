#pragma once

#include "gmredis/command/command_registry.h"
#include <unordered_map>

namespace gmredis::command {
    class DefaultCommandRegistry : public CommandRegistry {
    public:
        std::optional<CommandRegistryError> registerCommand(CommandType commandType, std::shared_ptr<Command> command) noexcept override;
        std::expected<std::shared_ptr<Command>, CommandRegistryError> getCommand(CommandType commandType) const noexcept override;
    private:
        std::unordered_map<CommandType, std::shared_ptr<Command>> commands;
    };
}
