#pragma once

#include "gmredis/command/command.h"
#include <expected>
#include <optional>
#include <memory>

namespace gmredis::command {

    enum class CommandRegistryError {
        AlreadyRegistered,
        CommandNotFound
    };

    class CommandRegistry {
        public:
            virtual ~CommandRegistry() = default;

            virtual std::optional<CommandRegistryError> registerCommand(CommandType, std::shared_ptr<Command>) noexcept = 0;

            virtual std::expected<std::shared_ptr<Command>, CommandRegistryError> getCommand(CommandType) noexcept = 0;
    };
}