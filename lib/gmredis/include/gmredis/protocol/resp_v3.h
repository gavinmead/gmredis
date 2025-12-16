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

    using RespValue = std::variant<SimpleString, SimpleError>;

}
