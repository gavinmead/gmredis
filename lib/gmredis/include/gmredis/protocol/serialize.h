#pragma once

#include "resp_v3.h"
#include <string>

namespace gmredis::protocol {

    std::string serialize(const SimpleString& resp);
    std::string serialize(const SimpleError& resp);
    std::string serialize(const RespValue& resp);

}