#pragma once

#include "gmredis/command/command_selector.h"
#include "gmredis/command/command_registry.h"
#include <memory>

namespace gmredis::command {

    constexpr size_t COMMAND_INDEX = 0;

    /**
     * @brief Default implementation of CommandSelector using a CommandRegistry for command lookup.
     *
     * DefaultCommandSelector provides a registry-based approach to selecting commands. It delegates
     * the responsibility of command instantiation to a CommandRegistry, which maintains the mapping
     * between command names and their corresponding Command implementations.
     *
     * This implementation extracts the command name from the RESP array and queries the registry
     * to obtain the appropriate Command instance.
     */
    class DefaultCommandSelector : public CommandSelector {
    public:
        /**
         * @brief Constructs a DefaultCommandSelector with the specified command registry.
         *
         * @param command_registry Unique pointer to the CommandRegistry that will be used
         *                        to look up and instantiate commands. Ownership is transferred
         *                        to this CommandSelector instance.
         */
        explicit DefaultCommandSelector(std::unique_ptr<CommandRegistry> command_registry) : command_registry(std::move(command_registry)) {}

        /**
         * @brief Selects a command by querying the registry with the command name from the request.
         *
         * @param req The RESP protocol array containing the command name and arguments.
         * @return std::expected containing either the selected Command or a CommandError.
         */
        std::expected<std::shared_ptr<Command>, CommandError> select(const protocol::Array& req) override;
    private:
        std::unique_ptr<CommandRegistry> command_registry; ///< Registry used for command lookup.

    };
}