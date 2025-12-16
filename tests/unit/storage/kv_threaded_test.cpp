
#include <gtest/gtest.h>
#include "storage/kv_mem.h"
#include "storage/kv_threading.h"
#include <memory>
#include <thread>


namespace gmredis::test {

    TEST(ThreadSafeKVTest, ConcurrentReadWrite) {
        auto store = std::make_unique<storage::ThreadSafeKVStore>(std::make_unique<storage::KVMemoryStore>());
        const int numThreads = 10;
        const int numOperations = 100;
        std::vector<std::thread> threads;

        // Writers
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back([&, i] {
                for (int j = 0; j < numOperations; ++j) {
                    store->put("key" + std::to_string(i), std::to_string(j));
                }
            });
        }

        // Readers
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back([&] {
                for (int j = 0; j < numOperations; ++j) {
                    [[maybe_unused]] auto _ = store->get("key0");
                }
            });
        }

        for (auto& t : threads) t.join();

        // Verify final state is consistent
        for (int i = 0; i < numThreads; ++i) {
            auto result = store->get("key" + std::to_string(i));
            EXPECT_TRUE(result.has_value());
        }

    }
}
