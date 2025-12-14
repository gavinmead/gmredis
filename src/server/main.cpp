#include <gmredis/version.h>

#include <print>

int main() {
    std::println("GMRedis Server");
    std::println("{}", gmredis::get_version_info());
    std::println("Starting server on port 6379...");

    // TODO: Implement server logic

    return 0;
}
