#pragma once

#include "gmredis/command/base_command.h"

namespace gmredis::command {
    /**
     * @brief Implementation of the Redis PING command.
     *
     * The PING command is used to test connectivity and verify that the server is responsive.
     * It supports two modes of operation:
     *
     * 1. **Simple ping**: Without arguments, returns "PONG" as a SimpleString
     * 2. **Echo mode**: With one argument, echoes back that argument as a BulkString
     *
     * This command is commonly used for:
     * - Checking if the connection to the server is alive
     * - Measuring latency between client and server
     * - Testing command processing in echo mode
     *
     * **Command format:**
     * - `PING` → Returns SimpleString "PONG"
     * - `PING <message>` → Returns BulkString containing <message>
     *
     * **Validation rules:**
     * - Accepts 0 or 1 arguments (plus the command name itself)
     * - All arguments must be BulkString type
     *
     * @example
     * ```cpp
     * auto pingCmd = PingCommand();
     *
     * // Simple ping
     * auto arg1 = protocol::Array{.values = {
     *     protocol::BulkString{.value = "PING", .length = 4}
     * }};
     * auto result1 = pingCmd.execute(arg1);
     * // Returns: SimpleString{.value = "PONG"}
     *
     * // Echo mode
     * auto arg2 = protocol::Array{.values = {
     *     protocol::BulkString{.value = "PING", .length = 4},
     *     protocol::BulkString{.value = "hello", .length = 5}
     * }};
     * auto result2 = pingCmd.execute(arg2);
     * // Returns: BulkString{.value = "hello", .length = 5}
     * ```
     *
     * @see BaseCommand for the Template Method pattern and execution lifecycle
     */
    class PingCommand : public BaseCommand {
    protected:
        /**
         * @brief Validates PING command arguments.
         *
         * Ensures that:
         * - The argument array contains at most 2 elements (command + optional argument)
         * - All arguments are of type BulkString
         *
         * @param arg The command arguments including the command name at index 0
         * @return std::nullopt if validation succeeds, or a CommandError if:
         *         - More than 1 argument is provided (WrongArgumentCount)
         *         - Any argument is not a BulkString (InvalidArgument)
         */
        std::optional<CommandError> doValidate(const protocol::Array& arg) override;

        /**
         * @brief Executes the PING command.
         *
         * Behavior depends on the number of arguments:
         * - If only the command name is present (arg.values.size() == 1):
         *   Returns SimpleString "PONG"
         * - If an argument is provided (arg.values.size() == 2):
         *   Returns the argument at index 1 as a BulkString (echo mode)
         *
         * @param arg The validated command arguments
         * @return Expected containing:
         *         - SimpleString{.value = "PONG"} for simple ping
         *         - BulkString with the echoed message for echo mode
         *
         * @note This method assumes validation has already succeeded
         */
        std::expected<protocol::RespValue, CommandError> doExecute(const protocol::Array& arg) override;
    };
}