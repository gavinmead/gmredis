#pragma once
#include <string>
#include <variant>

namespace gmredis::protocol {

    struct SimpleString {
        std::string value;
    };
    struct SimpleError {
        std::string value;
    };
    struct BulkString {
        std::string value;
        size_t length;
    };
    struct Integer {
        int64_t value;
    };

    using RespValue = std::variant<SimpleString,
                                   SimpleError,
                                   BulkString,
                                   Integer>;

}
