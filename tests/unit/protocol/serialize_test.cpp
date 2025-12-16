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

}
