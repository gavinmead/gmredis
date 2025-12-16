#include "gmredis/protocol/serialize.h"
#include <fmt/core.h>

namespace gmredis::protocol {
    std::string serialize(const SimpleString& resp) {
        return "+" + resp.value + "\r\n";
    }

    std::string serialize(const SimpleError& resp) {
        return "-" + resp.value + "\r\n";
    }

    std::string serialize(const BulkString& resp) {
        return fmt::format("${}\r\n{}\r\n", resp.length, resp.value);
    }

    std::string serialize(const RespValue& resp) {
        return std::visit([](const auto& value) {
            return serialize(value);
        }, resp);
    }
}  // namespace gmredis::protocol