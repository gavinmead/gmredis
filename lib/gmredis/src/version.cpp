#include "gmredis/version.h"

#include <format>

namespace gmredis {

std::string get_version_info() {
    return std::format("{} version {}", Version::name(), Version::string());
}

}  // namespace gmredis
