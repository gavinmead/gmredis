//
// Created by Gavin Mead on 12/15/25.
//

#pragma once

#include "gmredis/storage/kv.h"
#include <unordered_map>

namespace gmredis::storage {
    class KVMemoryStore : public KVStore {
    public:
        std::expected<void, ErrorInfo> put(const std::string &key, const std::string &value) override;
        std::expected<std::string, ErrorInfo> get(const std::string &key) override;
        std::expected<int, ErrorInfo> del(const std::string &key) override;

    private:
        std::unordered_map<std::string, std::string> store_;
    };
}
