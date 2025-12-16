#include "kv_threading.h"

namespace gmredis::storage {
    std::expected<void, ErrorInfo> ThreadSafeKVStore::put(const std::string &key, const std::string &value) {
        std::unique_lock const lock(mutex_);
        return store_->put(key, value);
    }

    std::expected<int, ErrorInfo> ThreadSafeKVStore::del(const std::string &key) {
        std::unique_lock const lock(mutex_);
        return store_->del(key);
    }

    std::expected<std::string, ErrorInfo> ThreadSafeKVStore::get(const std::string &key) {
        std::shared_lock const lock(mutex_);
        return store_->get(key);
    }

}