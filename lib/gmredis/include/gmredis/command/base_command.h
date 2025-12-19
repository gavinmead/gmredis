#pragma once

#include "command.h"

namespace gmredis::command {
    /**
     * @brief Template Method base class for implementing Redis commands.
     *
     * BaseCommand implements the Command interface using the Template Method design pattern.
     * It provides a framework for command execution with customizable hooks that run before
     * and after validation and execution phases.
     *
     * The execution flow is:
     * 1. validate() -> preValidate() -> doValidate() -> postValidate()
     * 2. execute() -> preExecute() -> doExecute() -> postExecute()
     *
     * Derived classes must implement:
     * - doValidate(): Core validation logic
     * - doExecute(): Core execution logic
     *
     * Derived classes may optionally override:
     * - preValidate(): Setup before validation (e.g., logging, metrics)
     * - postValidate(): Cleanup after validation
     * - preExecute(): Setup before execution (e.g., acquiring locks, logging)
     * - postExecute(): Cleanup after execution (e.g., releasing locks, metrics)
     *
     * @example
     * ```cpp
     * class PingCommand : public BaseCommand {
     * protected:
     *     std::optional<CommandError> doValidate(const protocol::Array& arg) override {
     *         if (arg.values.size() > 1) {
     *             return CommandError(CommandErrorCode::WrongArgumentCount,
     *                               "PING takes 0 or 1 arguments");
     *         }
     *         return std::nullopt;
     *     }
     *
     *     std::expected<protocol::RespValue, CommandError> doExecute(const protocol::Array& arg) override {
     *         if (arg.values.empty()) {
     *             return protocol::SimpleString("PONG");
     *         }
     *         return protocol::BulkString{.value = arg.values[0], .length=arg.values[0].length};
     *     }
     * };
     * ```
     */
    class BaseCommand : public Command {
    public:
        virtual ~BaseCommand() = default;

        /**
         * @brief Final implementation of Command::validate() using the Template Method pattern.
         *
         * This method orchestrates the validation flow by calling preValidate(), doValidate(),
         * and postValidate() in sequence. Cannot be overridden in derived classes.
         *
         * @param arg The command arguments as a RESP Array
         * @return std::nullopt if validation succeeds, or a CommandError on failure
         */
        std::optional<CommandError> validate(const protocol::Array& arg) final override;

        /**
         * @brief Final implementation of Command::execute() using the Template Method pattern.
         *
         * This method orchestrates the execution flow by calling preExecute(), doExecute(),
         * and postExecute() in sequence. Cannot be overridden in derived classes.
         *
         * @param arg The command arguments as a RESP Array
         * @return Expected containing the result on success, or a CommandError on failure
         */
        std::expected<protocol::RespValue, CommandError> execute(const protocol::Array& arg) final override;

    protected:
        /**
         * @brief Performs the core validation logic (must be implemented by derived classes).
         *
         * @param arg The command arguments to validate
         * @return std::nullopt if valid, or a CommandError describing the validation failure
         */
        virtual std::optional<CommandError> doValidate(const protocol::Array& arg) = 0;

        /**
         * @brief Performs the core execution logic (must be implemented by derived classes).
         *
         * @param arg The command arguments
         * @return Expected containing the command result or a CommandError on failure
         */
        virtual std::expected<protocol::RespValue, CommandError> doExecute(const protocol::Array& arg) = 0;

        /**
         * @brief Hook called before doValidate() (optional override).
         *
         * Use this for setup operations like logging or collecting metrics before validation.
         *
         * @param arg The command arguments
         */
        virtual void preValidate([[maybe_unused]] const protocol::Array& arg) {};

        /**
         * @brief Hook called after doValidate() completes (optional override).
         *
         * Called regardless of whether validation succeeded or failed.
         * Use this for cleanup operations after validation.
         *
         * @param arg The command arguments
         */
        virtual void postValidate([[maybe_unused]] const protocol::Array& arg) {};

        /**
         * @brief Hook called before doExecute() (optional override).
         *
         * Use this for setup operations like acquiring locks, logging, or metrics collection.
         *
         * @param arg The command arguments
         */
        virtual void preExecute([[maybe_unused]] const protocol::Array& arg) {};

        /**
         * @brief Hook called after doExecute() completes (optional override).
         *
         * Called regardless of whether execution succeeded or failed.
         * Use this for cleanup operations like releasing locks or recording metrics.
         * The result can be inspected or even modified before being returned to the caller.
         *
         * @param arg The command arguments
         * @param result The execution result (can be inspected or modified)
         */
        virtual void postExecute([[maybe_unused]] const protocol::Array& arg, [[maybe_unused]] std::expected<protocol::RespValue, CommandError>& result) {};

   };
}
