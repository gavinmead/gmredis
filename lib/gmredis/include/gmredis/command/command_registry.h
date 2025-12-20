#pragma once

#include "gmredis/command/command.h"
#include <expected>
#include <optional>
#include <memory>

namespace gmredis::command {

    /**
     * @brief Error codes for CommandRegistry operations.
     */
    enum class CommandRegistryError {
        /** Attempted to register a command type that is already registered */
        AlreadyRegistered,
        /** Attempted to retrieve a command type that is not registered */
        CommandNotFound,
        /** If null is passed in when registering a command*/
        NullCommand,
    };

    /**
     * @brief Registry interface for managing Redis command implementations.
     *
     * The CommandRegistry provides a centralized location for registering and retrieving
     * Command implementations by their CommandType. It manages the lifetime of registered
     * commands using shared_ptr for safe, shared ownership.
     *
     * Key responsibilities:
     * - Register command implementations with their corresponding CommandType
     * - Prevent duplicate registrations for the same CommandType
     * - Retrieve command implementations by CommandType
     * - Manage command lifetime through shared ownership
     *
     * @note Implementations should be thread-safe if used in a multi-threaded context.
     *
     * @example
     * ```cpp
     * auto registry = DefaultCommandRegistry();
     * auto pingCmd = std::make_shared<PingCommand>();
     *
     * // Register a command
     * auto registerResult = registry.registerCommand(CommandType::Ping, pingCmd);
     * if (registerResult.has_value()) {
     *     // Handle error: command already registered
     * }
     *
     * // Retrieve a command
     * auto getResult = registry.getCommand(CommandType::Ping);
     * if (getResult.has_value()) {
     *     auto cmd = getResult.value();
     *     // Use the command...
     * } else {
     *     // Handle error: command not found
     * }
     * ```
     */
    class CommandRegistry {
        public:
            virtual ~CommandRegistry() = default;

            /**
             * @brief Registers a command implementation for a specific CommandType.
             *
             * Associates the provided Command implementation with the given CommandType.
             * The registry shares ownership of the command via shared_ptr. If a command
             * is already registered for this type, registration fails.
             *
             * @param type The CommandType to register the command for
             * @param cmd Shared pointer to the Command implementation
             * @return std::nullopt on success, or CommandRegistryError::AlreadyRegistered
             *         if a command is already registered for this type
             *
             * @note This method is noexcept and will not throw exceptions
             */
            virtual std::optional<CommandRegistryError> registerCommand(CommandType, std::shared_ptr<Command>) noexcept = 0;

            /**
             * @brief Retrieves a registered command implementation by CommandType.
             *
             * Looks up and returns the Command implementation associated with the given
             * CommandType. Returns a shared_ptr to allow shared ownership of the command.
             *
             * @param type The CommandType to retrieve
             * @return Expected containing a shared_ptr to the Command on success, or
             *         CommandRegistryError::CommandNotFound if no command is registered
             *         for this type
             *
             * @note This method is noexcept and will not throw exceptions
             * @note The returned shared_ptr allows multiple callers to safely hold
             *       references to the same command
             */
            virtual std::expected<std::shared_ptr<Command>, CommandRegistryError> getCommand(CommandType) const noexcept = 0;
    };
}