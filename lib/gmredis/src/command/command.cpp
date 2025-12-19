#include "gmredis/command/command.h"
#include <optional>


namespace gmredis::command {
    std::optional<Command> get_command(std::string_view command) {
        if (command.empty()) {
            return std::nullopt;
        }

        auto it = command_table.find(std::string(command));
        if (it != command_table.end()) {
            return it->second;
        }

        return std::nullopt;
    };

}