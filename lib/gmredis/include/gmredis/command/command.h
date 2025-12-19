#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <algorithm>
#include <cctype>


namespace gmredis::command {

    enum class Command {
        Ping,
        Get,
        Set
    };

    struct CaseInsensitiveHash {
        std::size_t operator()(std::string_view sv) const {
            std::string lower(sv);
            std::transform(lower.begin(), lower.end(), lower.begin(),
                          [](unsigned char c) {
                              return static_cast<char>(std::tolower(c));
                          });
            return std::hash<std::string>{}(lower);

        }
    };

    struct CaseInsensitiveEqual {
        bool operator()(std::string_view a, std::string_view b) const {
            return std::equal(a.begin(), a.end(), b.begin(), b.end(),
                             [](char a, char b) {
                                 return std::tolower(static_cast<unsigned char>(a)) ==
                                        std::tolower(static_cast<unsigned char>(b));
                             });
        }
    };


    inline auto make_command_table() {
        std::unordered_map<std::string, Command, CaseInsensitiveHash, CaseInsensitiveEqual> command_map = {
            {"ping", Command::Ping},
            {"set", Command::Set},
            {"get", Command::Get}
        };

        return command_map;
    }

    inline const auto command_table = make_command_table();

    std::optional<Command> get_command(std::string_view command);

}