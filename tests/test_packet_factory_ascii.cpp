#include <gtest/gtest.h>
#include "PacketFactoryAsciiLines.h"
#include "MemoryBlock.h"

using namespace Threader::Threads;

static MemoryBlock::Ptr block(const std::string &s)
{
    auto b = std::make_shared<MemoryBlock>();
    b->append(s.c_str(), StdStringUtils::utf8Length(s));
    return b;
}

TEST(PacketFactoryAsciiLines, ExtractSingleLine)
{
    PacketFactoryAsciiLines factory;
    auto data = block("hello\r\n");

    auto packet = factory.tryExtractPacket(data);
    ASSERT_TRUE(packet);
    auto line = std::dynamic_pointer_cast<PacketAsciiLine>(packet);
    ASSERT_TRUE(line);
    EXPECT_EQ(line->line(), "hello");
    EXPECT_EQ(data->size(), 0); // терминатор и строка изъяты из буфера
}

TEST(PacketFactoryAsciiLines, NoTerminatorReturnsNull)
{
    PacketFactoryAsciiLines factory;
    std::string string = "partial line without terminator и русским текстом";
    auto stringSize = StdStringUtils::utf8Length(string);
    auto data = block(string.c_str());

    EXPECT_EQ(factory.tryExtractPacket(data), nullptr);
    EXPECT_EQ(data->size(), stringSize); // буфер не тронут
}

TEST(PacketFactoryAsciiLines, TwoPacketsInOneChunk)
{
    PacketFactoryAsciiLines factory;
    auto data = block("first\r\nsecond\r\n");

    auto p1 = factory.tryExtractPacket(data);
    ASSERT_TRUE(p1);
    EXPECT_EQ(std::dynamic_pointer_cast<PacketAsciiLine>(p1)->line(), "first");

    auto p2 = factory.tryExtractPacket(data);
    ASSERT_TRUE(p2);
    EXPECT_EQ(std::dynamic_pointer_cast<PacketAsciiLine>(p2)->line(), "second");

    EXPECT_EQ(factory.tryExtractPacket(data), nullptr);
}

TEST(PacketFactoryAsciiLines, EmptyBufferReturnsNull)
{
    PacketFactoryAsciiLines factory;
    auto data = std::make_shared<MemoryBlock>();
    EXPECT_EQ(factory.tryExtractPacket(data), nullptr);
}

TEST(PacketFactoryAsciiLines, BuildPacketAppendsCrlfOnWrite)
{
    PacketFactoryAsciiLines factory;
    auto packet = factory.buildPacket("ping 123", false);
    ASSERT_TRUE(packet);

    auto data = std::make_shared<MemoryBlock>();
    ASSERT_TRUE(packet->write(data));
    EXPECT_EQ(std::string((char *)data->data(), data->size()), "ping 123\r\n");
}
