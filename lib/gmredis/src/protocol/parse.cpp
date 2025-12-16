#include "gmredis/protocol/parse.h"
#include <expected>
#include <string>
#include <charconv>

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
        //This should be parsable to a int
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
