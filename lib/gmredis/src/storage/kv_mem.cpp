#include "kv_mem.h"
#include <format>
#include <spdlog/spdlog.h>

namespace gmredis::storage {

    std::expected<void, ErrorInfo> KVMemoryStore::put(const std::string &key, const std::string &value) {
        store_[key] = value;
        spdlog::debug("KVMemoryStore.put called with key: {}, value: {}", key, value);
        return {};
    }

    std::expected<int, ErrorInfo> KVMemoryStore::del([[maybe_unused]]const std::string &key) {
        return 0;
    }

    std::expected<std::string, ErrorInfo> KVMemoryStore::get(const std::string &key) {
        spdlog::debug("KVMemoryStore.get called with key: {}", key);
        auto result = store_.find(key);
        if (result == store_.end()) {
            spdlog::debug("KVMemoryStore.get called with key: {}, key not found", key);
            return std::unexpected{ErrorInfo(KVError::KeyNotFound, std::format("{} was not found", key))};
        }
        return result->second;
    }

}