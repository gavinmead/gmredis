#pragma once

#include "gmredis/command/command.h"
#include "gmredis/protocol/resp_v3.h"
#include <expected>
#include <memory>

#include "command_registry.h"

namespace gmredis::command {
    /**
     * @brief Interface for selecting and instantiating Redis commands based on RESP protocol requests.
     *
     * CommandSelector is responsible for parsing incoming RESP protocol arrays and determining
     * which Command implementation should handle the request. Implementations of this interface
     * typically maintain a registry of available commands and perform command name resolution.
     */
    class CommandSelector {
    public:
        virtual ~CommandSelector() = default;

        /**
         * @brief Selects and creates the appropriate Command instance for the given request.
         *
         * @param req The RESP protocol array containing the command name and arguments.
         *            Typically, the first element is the command name (e.g., "GET", "SET"),
         *            and subsequent elements are the command arguments.
         *
         * @return std::expected containing either:
         *         - A shared_ptr to the selected Command instance on success, or
         *         - A CommandError describing why the command could not be selected
         *           (e.g., unknown command, invalid format, parsing error)
         */
        virtual std::expected<std::shared_ptr<Command>, CommandError> select(const protocol::Array& req) = 0;

        CommandSelector() = default;

    };
}
