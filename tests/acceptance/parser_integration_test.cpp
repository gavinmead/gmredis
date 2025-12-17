#include <catch2/catch_test_macros.hpp>
#include "gmredis/protocol/parse.h"
#include "gmredis/protocol/resp_v3.h"

using namespace gmredis::protocol;

TEST_CASE("make_parser_table registers parsers correctly", "[parser]") {
    constexpr auto table = make_parser_table();

    SECTION("SimpleString parser is registered at '+'") {
        REQUIRE(table['+'] != nullptr);
    }

    SECTION("Error parser is registered at '-'") {
        REQUIRE(table['-'] != nullptr);
    }

    SECTION("Bulk parser is registered at '$'") {
        REQUIRE(table['$'] != nullptr);
    }

    SECTION("Integer parser is registered at ':'") {
        REQUIRE(table[':'] != nullptr);
    }

    SECTION("Array parser is registered at '*'") {
        REQUIRE(table['*'] != nullptr);
    }

}

TEST_CASE("Parser table dispatches to correct parser", "[parser]") {
    constexpr auto table = make_parser_table();

    SECTION("Dispatches '+' to SimpleString parser") {
        std::string_view input = "+OK\r\n";
        auto parser = table['+'];
        REQUIRE(parser != nullptr);

        auto result = parser(input);
        REQUIRE(result.has_value());
        REQUIRE(std::holds_alternative<SimpleString>(result.value()));
        REQUIRE(std::get<SimpleString>(result.value()).value == "OK");
        REQUIRE(input.empty());  // Input consumed
    }

    SECTION("Dispatches '-' to Error parser") {
        std::string_view input = "-ERR bad command\r\n";
        auto parser = table['-'];
        REQUIRE(parser != nullptr);

        auto result = parser(input);
        REQUIRE(result.has_value());
        REQUIRE(std::holds_alternative<SimpleError>(result.value()));
        REQUIRE(std::get<SimpleError>(result.value()).value == "ERR bad command");
        REQUIRE(input.empty());
    }
}

TEST_CASE("Parser table handles multiple messages in buffer", "[parser]") {
    constexpr auto table = make_parser_table();

    std::string_view input = "+OK\r\n-ERR fail\r\n+PONG\r\n";

    auto parser1 = table['+'];
    auto result1 = parser1(input);
    REQUIRE(result1.has_value());
    REQUIRE(std::get<SimpleString>(result1.value()).value == "OK");

    auto parser2 = table[static_cast<unsigned char>(input[0])];
    auto result2 = parser2(input);
    REQUIRE(result2.has_value());
    REQUIRE(std::get<SimpleError>(result2.value()).value == "ERR fail");

    auto parser3 = table[static_cast<unsigned char>(input[0])];
    auto result3 = parser3(input);
    REQUIRE(result3.has_value());
    REQUIRE(std::get<SimpleString>(result3.value()).value == "PONG");

    REQUIRE(input.empty());
}

TEST_CASE("Parser table handles incomplete messages", "[parser]") {
    constexpr auto table = make_parser_table();

    SECTION("Missing CRLF returns Incomplete") {
        std::string_view input = "+OK";
        auto parser = table['+'];
        auto result = parser(input);
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == ParseError::Incomplete);
    }

    SECTION("Empty input returns Incomplete") {
        std::string_view input = "";
        auto parser = table['+'];
        auto result = parser(input);
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == ParseError::Incomplete);
    }
}

SCENARIO("Parsing RESP arrays", "[parse][array]") {
    GIVEN("a valid RESP array with two bulk strings") {
        std::string_view input = "*2\r\n$5\r\nhello\r\n$5\r\nworld\r\n";

        WHEN("parse is called") {
            auto result = parse(input);

            THEN("it returns an Array") {
                REQUIRE(result.has_value());
                REQUIRE(std::holds_alternative<Array>(result.value()));

                AND_THEN("the array contains the expected values") {
                    auto array = std::get<Array>(result.value());
                    REQUIRE(array.values.size() == 2);
                }
            }
        }
    }

    GIVEN("an incomplete RESP array") {
        std::string_view input = "*2\r\n$5\r\nhello\r\n";

        WHEN("parse is called") {
            auto result = parse(input);

            THEN("it returns Incomplete error") {
                REQUIRE_FALSE(result.has_value());
                REQUIRE(result.error() == ParseError::Incomplete);
            }
        }
    }
}
