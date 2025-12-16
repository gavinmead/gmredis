#pragma once

#include "gmredis/storage/kv.h"
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace gmredis::storage {
    class ThreadSafeKVStore : public KVStore {
    public:
        explicit ThreadSafeKVStore(std::unique_ptr<KVStore> store) : store_(std::move(store)) {}
        std::expected<void, ErrorInfo> put(const std::string &key, const std::string &value) override;
        std::expected<std::string, ErrorInfo> get(const std::string &key) override;
        std::expected<int, ErrorInfo> del(const std::string &key) override;
    private:
        std::unique_ptr<KVStore> store_;
        mutable std::shared_mutex mutex_;
    };
}