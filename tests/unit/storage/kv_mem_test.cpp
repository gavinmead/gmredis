#include <gtest/gtest.h>

#include "storage/kv_mem.h"

namespace gmredis::test {
    TEST(KVMemoryStoreTest, DefaultConstructor) {
        storage::KVMemoryStore store;
        EXPECT_TRUE(true); // If we reach here, the constructor worked.
    }

    TEST(KVMemoryStoreTest, GetNonExistentKey) {
        storage::KVMemoryStore store;
        auto getResult = store.get("nonexistent_key");
        EXPECT_FALSE(getResult.has_value());
        EXPECT_EQ(getResult.error().code, storage::KVError::KeyNotFound);
        EXPECT_EQ(getResult.error().message, "nonexistent_key was not found");
    }

    TEST(KVMemoryStoreTest, PutAndGet) {
        storage::KVMemoryStore store;
        auto putResult = store.put("key1", "value1");
        EXPECT_TRUE(putResult.has_value());

        auto getResult = store.get("key1");
        EXPECT_TRUE(getResult.has_value());
        EXPECT_EQ(getResult.value(), "value1");
    }

    TEST(KVMemoryStoreTest, PutAndGetEmptyString) {
        storage::KVMemoryStore store;
        auto putResult = store.put("empty_key", "");
        EXPECT_TRUE(putResult.has_value());

        auto getResult = store.get("empty_key");
        EXPECT_TRUE(getResult.has_value());
        EXPECT_EQ(getResult.value(), "");
    }

    TEST(KVMemoryStoreTest, OverwriteValue) {
        storage::KVMemoryStore store;
        auto putResult1 = store.put("key1", "value1");
        EXPECT_TRUE(putResult1.has_value());

        auto putResult2 = store.put("key1", "value2");
        EXPECT_TRUE(putResult2.has_value());

        auto getResult = store.get("key1");
        EXPECT_TRUE(getResult.has_value());
        EXPECT_EQ(getResult.value(), "value2");
    }
}
