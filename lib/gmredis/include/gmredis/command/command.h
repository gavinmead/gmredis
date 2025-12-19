#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <algorithm>
#include <cctype>
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
    };

    struct CommandError {
        CommandErrorCode code;
        std::string message;

        CommandError(CommandErrorCode c, std::string msg)
            : code(c), message(std::move(msg)) {}
    };

    class Command {
    public:
        virtual ~Command() = default;
        virtual std::expected<protocol::RespValue, CommandError> validate(const protocol::RespValue&) = 0;
        virtual std::expected<protocol::RespValue, CommandError> execute(const protocol::RespValue&) = 0;
    };
}