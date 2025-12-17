#include <gtest/gtest.h>
#include "gmredis/protocol/serialize.h"
#include "gmredis/protocol/resp_v3.h"

namespace gmredis::test {
    TEST(SerializeTest, SimpleStringSerialization) {
        protocol::SimpleString const simple_string{"Hello, World!"};
        auto serialized = protocol::serialize(simple_string);
        EXPECT_EQ(serialized, "+Hello, World!\r\n");
    }

    TEST(SerializeTest, EmptySimpleStringSerialization) {
        protocol::SimpleString const simple_string{""};
        auto serialized = protocol::serialize(simple_string);
        EXPECT_EQ(serialized, "+\r\n");
    }

    TEST(SerializeTest, SimpleErrorSerialization) {
        protocol::SimpleError const simple_error{"ERR Something went wrong"};
        auto serialized = protocol::serialize(simple_error);
        EXPECT_EQ(serialized, "-ERR Something went wrong\r\n");
    }

    TEST(SerializeTest, RespValueSimpleStringSerialization) {
        protocol::RespValue const resp_value = protocol::SimpleString{"Hello, RespValue!"};
        auto serialized = protocol::serialize(resp_value);
        EXPECT_EQ(serialized, "+Hello, RespValue!\r\n");
    }

    TEST(SerializeTest, BulkStringSerialization) {
        protocol::BulkString const bulk_string{.value = "Hello, World!", .length = 13};
        auto serialized = protocol::serialize(bulk_string);
        EXPECT_EQ(serialized, "$13\r\nHello, World!\r\n");
    }

    TEST(SerializeTest, IntegerSerializeOk) {
        protocol::Integer const integer_value{123};
        auto serialized = protocol::serialize(integer_value);
        EXPECT_EQ(serialized, ":123\r\n");
    }

    TEST(SerializeTest, IntegerSerializeNegativeOk) {
        protocol::Integer const integer_value{-123};
        auto serialized = protocol::serialize(integer_value);
        EXPECT_EQ(serialized, ":-123\r\n");
    }

    TEST(SerializeTest, ArraySerializeSameType) {
        protocol::BulkString const hello{.value = "hello", .length = 5};
        protocol::BulkString const world{.value = "world", .length = 5};
        protocol::BulkString const bulk{.value = "bulk", .length = 4};

        protocol::Array values = protocol::Array{.values = {hello, world, bulk}};

        auto serialized = protocol::serialize(values);
        EXPECT_EQ(serialized, "*3\r\n$5\r\nhello\r\n$5\r\nworld\r\n$4\r\nbulk\r\n");
    }

    TEST(SerializeTest, ArraySerializeMultiType) {
        protocol::BulkString const hello{.value = "hello", .length = 5};
        protocol::BulkString const world{.value = "world", .length = 5};
        protocol::Integer const integer_value{123};
        protocol::BulkString const bulk{.value = "bulk", .length = 4};

        protocol::Array values = protocol::Array{.values = {hello, world, integer_value, bulk}};

        auto serialized = protocol::serialize(values);
        EXPECT_EQ(serialized, "*4\r\n$5\r\nhello\r\n$5\r\nworld\r\n:123\r\n$4\r\nbulk\r\n");
    }

    TEST(SerializeTest, ArraySerializeEmpty) {
        protocol::Array values = protocol::Array{.values = {}};
        auto serialized = protocol::serialize(values);
        EXPECT_EQ(serialized, "*0\r\n");
    }

    TEST(SerializeTest, ArraySerializeNested) {
        protocol::BulkString const hello{.value = "hello", .length = 5};
        protocol::Array inner = protocol::Array{.values = {hello}};
        protocol::Array outer = protocol::Array{.values = {inner}};
        auto serialized = protocol::serialize(outer);
        EXPECT_EQ(serialized, "*1\r\n*1\r\n$5\r\nhello\r\n");
    }

    TEST(SerializeTest, ArraySerializeAllTypes) {
        protocol::SimpleString ss{.value = "OK"};
        protocol::SimpleError se{.value = "ERR error"};
        protocol::BulkString bs{.value = "bulk", .length = 4};
        protocol::Integer i{42};
        protocol::Array arr = protocol::Array{.values = {ss, se, bs, i}};
        auto serialized = protocol::serialize(arr);
        EXPECT_EQ(serialized, "*4\r\n+OK\r\n-ERR error\r\n$4\r\nbulk\r\n:42\r\n");
    }

    TEST(SerializeTest, RespValueArraySerialization) {
        protocol::BulkString const hello{.value = "hello", .length = 5};
        protocol::Array arr = protocol::Array{.values = {hello}};
        protocol::RespValue resp_value = arr;
        auto serialized = protocol::serialize(resp_value);
        EXPECT_EQ(serialized, "*1\r\n$5\r\nhello\r\n");
    }

    TEST(SerializeTest, ArraySerializeSingleElement) {
        protocol::Integer const integer_value{123};
        protocol::Array arr = protocol::Array{.values = {integer_value}};
        auto serialized = protocol::serialize(arr);
        EXPECT_EQ(serialized, "*1\r\n:123\r\n");
    }
}
