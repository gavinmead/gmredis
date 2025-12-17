#include "gmredis/protocol/parse.h"
#include <expected>
#include <string>
#include <charconv>
#include <vector>

namespace gmredis::protocol {
    std::expected<RespValue, ParseError> parse_simple_string([[maybe_unused]]std::string_view& input) {
        if (input.empty()) {
            return std::unexpected{ParseError::Incomplete};
        }

        if (!input.starts_with("+")) {
            return std::unexpected{ParseError::Invalid};
        }

        auto crlf = input.find("\r\n");
        if (crlf == std::string_view::npos) {
            return std::unexpected{ParseError::Incomplete};
        }

        std::string const value{input.substr(1, crlf - 1)};
        input.remove_prefix(crlf + 2);
        return SimpleString{value};
    }

    std::expected<RespValue, ParseError> parse_error(std::string_view& input) {
        if (input.empty()) {
            return std::unexpected{ParseError::Incomplete};
        }

        if (!input.starts_with("-")) {
            return std::unexpected{ParseError::Invalid};
        }

        auto crlf = input.find("\r\n");
        if (crlf == std::string_view::npos) {
            return std::unexpected{ParseError::Incomplete};
        }

        std::string const value{input.substr(1, crlf - 1)};
        input.remove_prefix(crlf + 2);
        return SimpleError{value};
    }

    std::expected<RespValue, ParseError> parse_bulk_string(std::string_view &input) {
        if (input.empty()) {
            return std::unexpected{ParseError::Incomplete};
        }

        if (!input.starts_with("$")) {
            return std::unexpected{ParseError::Invalid};
        }

        //First thing we do is find the first crlf; keep that value
        //Get the substring from 1-crlf-1, which will tell us the length of the string
        //Next, starting from crlf + 1, find the next crlf to get the string

        auto crlf = input.find("\r\n");
        if (crlf == std::string_view::npos) {
            return std::unexpected{ParseError::Incomplete};
        }

        auto next = crlf + 2;

        auto const length_str {input.substr(1, crlf - 1)};

        size_t length = 0;
        auto [ptr, ec] = std::from_chars(length_str.data(), length_str.data() + length_str.size(), length);

        if (ec != std::errc()) {
            return std::unexpected{ParseError::Incomplete};
        }

        std::string const value{input.substr(next,  length)};

        //Get the next crlf to ensure we are done
        crlf = input.find("\r\n", next + length);
        if (crlf == std::string_view::npos) {
            return std::unexpected{ParseError::Incomplete};
        }
        input.remove_prefix(crlf + 2);

        return BulkString{.value=value, .length=length};
    }

    std::expected<RespValue, ParseError> parse_integer(std::string_view &input) {
        if (input.empty()) {
            return std::unexpected{ParseError::Incomplete};
        }

        if (!input.starts_with(":")) {
            return std::unexpected{ParseError::Invalid};
        }

        auto crlf = input.find("\r\n");
        if (crlf == std::string_view::npos) {
            return std::unexpected{ParseError::Incomplete};
        }

        int i_value = 0;

        size_t start = (input[1] == '+') ? 2 : 1;
        auto [ptr, ec] = std::from_chars(input.data() + start, input.data() + crlf, i_value);

        if (ec != std::errc()) {
            return std::unexpected{ParseError::Invalid};
        }

        if (ptr != input.data() + crlf) {
            return std::unexpected{ParseError::Invalid};
        }

        input.remove_prefix(crlf + 2);

        return Integer{i_value};
    }

    std::expected<RespValue, ParseError> parse_array(std::string_view &input) {
        if (input.empty()) {
            return std::unexpected{ParseError::Incomplete};
        }

        if (!input.starts_with("*")) {
            return std::unexpected{ParseError::Invalid};
        }

        auto crlf = input.find("\r\n");
        if (crlf == std::string_view::npos) {
            return std::unexpected{ParseError::Incomplete};
        }

        std::string const length_str{input.substr(1, crlf - 1)};

        size_t array_length = 0;
        auto [ptr, ec] = std::from_chars(length_str.data(), length_str.data() + length_str.size(), array_length);

        if (ec != std::errc()) {
            return std::unexpected{ParseError::Invalid};
        }

        input.remove_prefix(crlf + 2);
        std::vector<RespValue> array_values = {};
        array_values.reserve(array_length);

        for (size_t i = 0; i < array_length; i++) {
            auto result = parse(input);
            if (!result.has_value()) {
                return std::unexpected{result.error()};
            }

            array_values.push_back(*std::move(result));
        }

        return Array{array_values};
    }

    std::expected<RespValue, ParseError> parse(std::string_view& input) {
        if (input.empty()) {
            return std::unexpected{ParseError::Incomplete};
        }

        // Get the first character to determine the type
        char const prefix = input.front();
        auto const parser = parser_table[static_cast<unsigned char>(prefix)];
        if (parser == nullptr) {
            return std::unexpected{ParseError::Unsupported};
        }

        return parser(input);
    }
}
