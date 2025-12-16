#ifndef GMREDIS_KV_H
#define GMREDIS_KV_H

#include <optional>
#include <string>
#include <expected>

namespace gmredis::storage {

    enum class KVError {
        KeyNotFound,
        StorageFull,
        PutError,
        UnknownError
    };

    struct ErrorInfo {
        KVError code;
        std::string message;
    };

    class KVStore {
    public:

        virtual ~KVStore() = default;
        virtual std::expected<void, ErrorInfo> put(const std::string &key, const std::string &value) = 0;
        virtual std::expected<std::string, ErrorInfo> get(const std::string &key) = 0;
        virtual std::expected<int, ErrorInfo> del(const std::string &key) = 0;
    };
}

#endif //GMREDIS_KV_H