#pragma once

#include <string_view>

namespace gmredis {

struct Version {
    static constexpr int major = 0;
    static constexpr int minor = 1;
    static constexpr int patch = 0;

    [[nodiscard]] static constexpr std::string_view string() noexcept {
        return "0.1.0";
    }

    [[nodiscard]] static constexpr std::string_view name() noexcept {
        return "gmredis";
    }
};

[[nodiscard]] std::string get_version_info();

}  // namespace gmredis
