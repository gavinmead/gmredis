#pragma once
#include <string>
#include <variant>
#include <vector>

namespace gmredis::protocol {

    struct SimpleString {
        std::string value;

        bool operator==(const SimpleString&) const = default;
    };
    struct SimpleError {
        std::string value;

        bool operator==(const SimpleError&) const = default;
    };
    struct BulkString {
        std::string value;
        size_t length;

        bool operator==(const BulkString&) const = default;

    };
    struct Integer {
        int64_t value;

        bool operator==(const Integer&) const = default;
    };

    struct Array;

    using RespValue = std::variant<SimpleString,
                                   SimpleError,
                                   BulkString,
                                   Integer,
                                   Array>;

    struct Array {
        std::vector<RespValue> values;

        bool operator==(const Array&) const = default;
    };

}
