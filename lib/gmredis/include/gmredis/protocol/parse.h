#pragma once

#include "gmredis/protocol/resp_v3.h"
#include <string_view>
#include <expected>
#include <array>

namespace gmredis::protocol {
    enum class ParseError {
        Incomplete,
        Invalid,
        Unsupported
    };

    using Parser = std::expected<RespValue, ParseError>(*)(std::string_view&);

    std::expected<RespValue, ParseError> parse_simple_string(std::string_view &input);
    std::expected<RespValue, ParseError> parse_error(std::string_view &input);
    std::expected<RespValue, ParseError> parse_bulk_string(std::string_view &input);

    constexpr auto make_parser_table() {
        std::array<Parser, 128> table{};
        table['+'] = parse_simple_string;
        table['-'] = parse_error;
        table['$'] = parse_bulk_string;
        return table;
    }

    inline constexpr auto parser_table = make_parser_table();

    std::expected<RespValue, ParseError> parse(std::string_view &input);

}