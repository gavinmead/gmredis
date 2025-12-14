#include <gmredis/version.h>

#include <print>

int main() {
    std::println("GMRedis CLI");
    std::println("{}", gmredis::get_version_info());

    // TODO: Implement client logic

    return 0;
}
