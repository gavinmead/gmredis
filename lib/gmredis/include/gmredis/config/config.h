#pragma once

#include <string>
#include <expected>
#include <optional>

namespace gmredis::config {
    enum class ConfigurationErrorCode {
        InvalidConfiguration
    };

    struct ConfigurationError {
        ConfigurationErrorCode errorCode;
        std::string message;
    };

    class ServerConfiguration {
    public:
        static std::expected<ServerConfiguration, ConfigurationError> createServerConfiguration();
        static std::expected<ServerConfiguration, ConfigurationError> createServerConfiguration(std::optional<std::string> host, std::optional<int> port);

        const std::string& getHost();
        const int& getPort();

    private:
        ServerConfiguration(std::string const& host, int port) : host(host), port(port) {}

        std::string host;

        int port;
    };

    // struct RequestConfiguration {
    //     std::string max_request_size;
    //     std::string max_string_size;
    // };

    // struct SecurityConfiguration {
    //     bool tls_enabled;
    //     std::string certificate_file;
    //     std::string private_key_file;
    //     bool verify_peer_cert;
    // };

    // class Configuration {
    // public:
    //     Configuration();
    //
    //     const ServerConfiguration& getServerConfiguration();
    //     const RequestConfiguration& getRequestConfiguration();
    //     const SecurityConfiguration& getSecurityConfiguration();
    // };
}