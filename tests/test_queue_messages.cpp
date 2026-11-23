#include <gtest/gtest.h>
#include "QueueMessages.h"
#include "MessageBase.h"

using namespace Threader::Threads;

TEST(QueueMessages, EmptyQueueDequeueReturnsNull)
{
    QueueMessages queue;
    EXPECT_EQ(queue.count(), 0);
    EXPECT_EQ(queue.dequeue(), nullptr);
}

TEST(QueueMessages, EnqueueThenDequeuePreservesOrder)
{
    QueueMessages queue;
    auto first = std::make_shared<MessageBase>("m.first");
    auto second = std::make_shared<MessageBase>("m.second");

    queue.enqueue(first);
    queue.enqueue(second);

    EXPECT_EQ(queue.count(), 2);
    EXPECT_EQ(queue.dequeue()->name(), "m.first");
    EXPECT_EQ(queue.dequeue()->name(), "m.second");
    EXPECT_EQ(queue.count(), 0);
}

TEST(QueueMessages, DequeueAllDrainsQueue)
{
    QueueMessages queue;
    for (int i = 0; i < 5; i++)
        queue.enqueue(std::make_shared<MessageBase>("m." + std::to_string(i)));

    auto all = queue.dequeueAll();
    EXPECT_EQ(all.size(), 5);
    EXPECT_EQ(queue.count(), 0);
    EXPECT_TRUE(queue.dequeueAll().empty());
}
