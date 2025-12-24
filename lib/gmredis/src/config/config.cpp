#include "gmredis/config/config.h"
#include <algorithm>

namespace gmredis::config {

    std::expected<ServerConfiguration, ConfigurationError> ServerConfiguration::createServerConfiguration() {
        return createServerConfiguration(std::nullopt, std::nullopt);
    };

    std::expected<ServerConfiguration, ConfigurationError> ServerConfiguration::createServerConfiguration(std::optional<std::string> host, std::optional<int> port) {
        auto host_name_to_use = host.value_or("0.0.0.0");
        auto port_to_use = port.value_or(6379);

        if (host_name_to_use.empty()) {
            return std::unexpected(ConfigurationError{ConfigurationErrorCode::InvalidConfiguration, "host cannot be empty"});
        }

        auto empty_space_only = std::ranges::all_of(host_name_to_use,[](unsigned char c){return std::isspace(c);});

        if (empty_space_only) {
            return std::unexpected(ConfigurationError{ConfigurationErrorCode::InvalidConfiguration, "host name cannot be whitespace"});
        }

        if (port_to_use <= 0 || port_to_use > 65535) {
            return std::unexpected(ConfigurationError{ConfigurationErrorCode::InvalidConfiguration, "port number is not valid"});
        }

        return ServerConfiguration(host_name_to_use, port_to_use);
    };

    const std::string& ServerConfiguration::getHost() {
        return host;
    }

    const int& ServerConfiguration::getPort() {
        return port;
    }

}