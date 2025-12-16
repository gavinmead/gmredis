#include "gmredis/protocol/parse.h"
#include <expected>
#include <string>

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