#pragma once

#include "gmredis/protocol/resp_v3.h"
#include <string_view>
#include <expected>
#include <array>

namespace gmredis::protocol {
    enum class ParseError {
        Incomplete,
        Invalid,
    };

    using Parser = std::expected<RespValue, ParseError>(*)(std::string_view&);

    std::expected<RespValue, ParseError> parse_simple_string(std::string_view &input);
    std::expected<RespValue, ParseError> parse_error(std::string_view &input);

    constexpr auto make_parser_table() {
        std::array<Parser, 256> table{};
        table['+'] = parse_simple_string;
        table['-'] = parse_error;
        return table;
    }
}