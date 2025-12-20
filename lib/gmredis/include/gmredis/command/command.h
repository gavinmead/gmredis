#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <algorithm>
#include <expected>
#include "gmredis/protocol/resp_v3.h"


namespace gmredis::command {

    enum class CommandType {
        Ping,
        Get,
        Set
    };

    struct CaseInsensitiveHash {
        std::size_t operator()(std::string_view sv) const {
            std::size_t hash = 0;
            for (char c : sv) {
                hash = hash * 31 + static_cast<std::size_t>(std::tolower(static_cast<unsigned char>(c)));
            }
            return hash;

        }
    };

    struct CaseInsensitiveEqual {
        bool operator()(std::string_view a, std::string_view b) const {
            if (a.size() != b.size()) return false;
            return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                             [](char a, char b) {
                                 return std::tolower(static_cast<unsigned char>(a)) ==
                                        std::tolower(static_cast<unsigned char>(b));
                             });
        }
    };


    inline auto make_command_table() {
        std::unordered_map<std::string_view, CommandType, CaseInsensitiveHash, CaseInsensitiveEqual> command_map = {
            {"ping", CommandType::Ping},
            {"set", CommandType::Set},
            {"get", CommandType::Get}
        };

        return command_map;
    }

    inline const auto command_table = make_command_table();

    std::optional<CommandType> get_command(std::string_view command);

    enum class CommandErrorCode {
        InvalidArgument,
        WrongArgumentCount,
        KeyNotFound,
        ExecutionFailed,
        UnknownError,
        CommandNotFound,
    };

    struct CommandError {
        CommandErrorCode code;
        std::string message;

        CommandError(CommandErrorCode c, std::string msg)
            : code(c), message(std::move(msg)) {}
    };

    /**
     * @brief Abstract base interface for all Redis commands.
     *
     * The Command interface defines the contract that all Redis command implementations
     * must follow. Each command must support two core operations:
     * 1. Validation of command arguments
     * 2. Execution of the command logic
     *
     * Commands follow a two-phase execution model:
     * - validate() checks if the provided arguments are valid before execution
     * - execute() performs the actual command logic
     *
     * @note For most use cases, prefer deriving from BaseCommand instead of implementing
     *       this interface directly. BaseCommand provides a Template Method pattern with
     *       pre/post hooks for validation and execution.
     */
    class Command {
    public:
        virtual ~Command() = default;

        /**
         * @brief Validates the command arguments.
         *
         * This method checks if the provided arguments are valid for this command.
         * It should verify argument count, types, and any other preconditions
         * without modifying any state.
         *
         * @param arg The command arguments as a RESP Array
         * @return std::nullopt if validation succeeds, or a CommandError describing
         *         the validation failure
         */
        virtual std::optional<CommandError> validate(const protocol::Array&) = 0;

        /**
         * @brief Executes the command with the provided arguments.
         *
         * This method performs the actual command logic. It should only be called
         * after validate() has succeeded.
         *
         * @param arg The command arguments as a RESP Array
         * @return Expected containing the command result as a RespValue on success,
         *         or a CommandError on failure
         */
        virtual std::expected<protocol::RespValue, CommandError> execute(const protocol::Array&) = 0;
    };
}